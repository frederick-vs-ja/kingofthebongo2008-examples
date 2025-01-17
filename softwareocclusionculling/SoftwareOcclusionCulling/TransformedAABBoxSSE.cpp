//--------------------------------------------------------------------------------------
// Copyright 2013 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//
//--------------------------------------------------------------------------------------

#include "TransformedAABBoxSSE.h"

static const UINT sBBIndexList[AABB_INDICES] =
{
	// index for top 
	1, 3, 2,
	0, 3, 1,

	// index for bottom
	5, 7, 4,
	6, 7, 5,

	// index for left
	1, 7, 6,
	2, 7, 1,

	// index for right
	3, 5, 4,
	0, 5, 3,

	// index for back
	2, 4, 7,
	3, 4, 2,

	// index for front
	0, 6, 5,
	1, 6, 0,
};

// 0 = use min corner, 1 = use max corner
static const UINT sBBxInd[AABB_VERTICES] = { 1, 0, 0, 1, 1, 1, 0, 0 };
static const UINT sBByInd[AABB_VERTICES] = { 1, 1, 1, 1, 0, 0, 0, 0 };
static const UINT sBBzInd[AABB_VERTICES] = { 1, 1, 0, 0, 0, 1, 1, 0 };

//--------------------------------------------------------------------------
// Get the bounding box center and half vector
// Create the vertex and index list for the triangles that make up the bounding box
//--------------------------------------------------------------------------
void TransformedAABBoxSSE::CreateAABBVertexIndexList(CPUTModelDX11 *pModel)
{
	mWorldMatrix = *pModel->GetWorldMatrix();

	pModel->GetBoundsObjectSpace(&mBBCenter, &mBBHalf);
	mRadiusSq = mBBHalf.lengthSq();
	pModel->GetBoundsWorldSpace(&mBBCenterWS, &mBBHalfWS);
}

//----------------------------------------------------------------
// Determine is model is inside view frustum
//----------------------------------------------------------------
bool TransformedAABBoxSSE::IsInsideViewFrustum(CPUTCamera *pCamera)
{
	return pCamera->mFrustum.IsVisible(mBBCenterWS, mBBHalfWS);
}

//----------------------------------------------------------------------------
// Determine if the occluddee size is too small and if so avoid drawing it
//----------------------------------------------------------------------------
bool TransformedAABBoxSSE::IsTooSmall(const BoxTestSetupSSE &setup, __m128 cumulativeMatrix[4])
{
	__m128 worldMatrix[4];
	worldMatrix[0] = ssp_loadu_ps(mWorldMatrix.r0.f);
	worldMatrix[1] = ssp_loadu_ps(mWorldMatrix.r1.f);
	worldMatrix[2] = ssp_loadu_ps(mWorldMatrix.r2.f);
	worldMatrix[3] = ssp_loadu_ps(mWorldMatrix.r3.f);
	MatrixMultiply(worldMatrix, setup.mViewProjViewport, cumulativeMatrix);
	
	float w = mBBCenter.x * cumulativeMatrix[0].m128_f32[3] + 
		      mBBCenter.y * cumulativeMatrix[1].m128_f32[3] + 
			  mBBCenter.z * cumulativeMatrix[2].m128_f32[3] + 
			  cumulativeMatrix[3].m128_f32[3];

	if( w > 1.0f )
	{
		return mRadiusSq < w * setup.radiusThreshold;
	}
	return false;
}

//----------------------------------------------------------------
// Trasforms the AABB vertices to screen space once every frame
//----------------------------------------------------------------
bool TransformedAABBoxSSE::TransformAABBox(__m128 xformedPos[], const __m128 cumulativeMatrix[4])
{
	// w ends up being garbage, but it doesn't matter - we ignore it anyway.
	__m128 vCenter = ssp_loadu_ps(&mBBCenter.x);
	__m128 vHalf   = ssp_loadu_ps(&mBBHalf.x);

	__m128 vMin    = ssp_sub_ps(vCenter, vHalf);
	__m128 vMax    = ssp_add_ps(vCenter, vHalf);

	// transforms
	__m128 xRow[2], yRow[2], zRow[2];
	xRow[0] = ssp_shuffle_ps(vMin, vMin, 0x00) * cumulativeMatrix[0];
	xRow[1] = ssp_shuffle_ps(vMax, vMax, 0x00) * cumulativeMatrix[0];
	yRow[0] = ssp_shuffle_ps(vMin, vMin, 0x55) * cumulativeMatrix[1];
	yRow[1] = ssp_shuffle_ps(vMax, vMax, 0x55) * cumulativeMatrix[1];
	zRow[0] = ssp_shuffle_ps(vMin, vMin, 0xaa) * cumulativeMatrix[2];
	zRow[1] = ssp_shuffle_ps(vMax, vMax, 0xaa) * cumulativeMatrix[2];

	__m128 zAllIn = ssp_castsi128_ps(ssp_set1_epi32(~0));

	for(UINT i = 0; i < AABB_VERTICES; i++)
	{
		// Transform the vertex
		__m128 vert = cumulativeMatrix[3];
		vert += xRow[sBBxInd[i]];
		vert += yRow[sBByInd[i]];
		vert += zRow[sBBzInd[i]];

		// We have inverted z; z is in front of near plane iff z <= w.
		__m128 vertZ = ssp_shuffle_ps(vert, vert, 0xaa); // vert.zzzz
		__m128 vertW = ssp_shuffle_ps(vert, vert, 0xff); // vert.wwww
		__m128 zIn = ssp_cmple_ps(vertZ, vertW);
		zAllIn = ssp_and_ps(zAllIn, zIn);

		// project
		xformedPos[i] = ssp_div_ps(vert, vertW);
	}

	// return true if and only if none of the verts are z-clipped
	return ssp_movemask_ps(zAllIn) == 0xf;
}

void TransformedAABBoxSSE::Gather(vFloat4 pOut[3], UINT triId, const __m128 xformedPos[], UINT idx)
{
	for(int i = 0; i < 3; i++)
	{
		UINT ind0 = sBBIndexList[triId*3 + i + 0];
		UINT ind1 = sBBIndexList[triId*3 + i + 3];
		UINT ind2 = sBBIndexList[triId*3 + i + 6];
		UINT ind3 = sBBIndexList[triId*3 + i + 9];

		__m128 v0 = xformedPos[ind0];	// x0 y0 z0 w0
		__m128 v1 = xformedPos[ind1];	// x1 y1 z1 w1
		__m128 v2 = xformedPos[ind2];	// x2 y2 z2 w2
		__m128 v3 = xformedPos[ind3];	// x3 y3 z3 w3
		_MM_TRANSPOSE4_PS(v0, v1, v2, v3);
		pOut[i].X = v0;
		pOut[i].Y = v1;
		pOut[i].Z = v2;
		pOut[i].W = v3;
	}
}

//-----------------------------------------------------------------------------------------
// Rasterize the occludee AABB and depth test it against the CPU rasterized depth buffer
// If any of the rasterized AABB pixels passes the depth test exit early and mark the occludee
// as visible. If all rasterized AABB pixels are occluded then the occludee is culled
//-----------------------------------------------------------------------------------------
bool TransformedAABBoxSSE::RasterizeAndDepthTestAABBox(UINT *pRenderTargetPixels, const __m128 pXformedPos[], UINT idx)
{
	// Set DAZ and FZ MXCSR bits to flush denormals to zero (i.e., make it faster)
	// Denormal are zero (DAZ) is bit 6 and Flush to zero (FZ) is bit 15. 
	// so to enable the two to have to set bits 6 and 15 which 1000 0000 0100 0000 = 0x8040
	ssp_setcsr( ssp_getcsr() | 0x8040 );

	__m128i colOffset = ssp_setr_epi32(0, 1, 0, 1);
	__m128i rowOffset = ssp_setr_epi32(0, 0, 1, 1);

	float* pDepthBuffer = (float*)pRenderTargetPixels; 
	
	// Rasterize the AABB triangles 4 at a time
	for(UINT i = 0; i < AABB_TRIANGLES; i += SSE)
	{
		vFloat4 xformedPos[3];
		Gather(xformedPos, i, pXformedPos, idx);

		// use fixed-point only for X and Y.  Avoid work for Z and W.
        __m128i fxPtX[3], fxPtY[3];
		for(int m = 0; m < 3; m++)
		{
			fxPtX[m] = ssp_cvtps_epi32(xformedPos[m].X);
			fxPtY[m] = ssp_cvtps_epi32(xformedPos[m].Y);
		}

		// Fab(x, y) =     Ax       +       By     +      C              = 0
		// Fab(x, y) = (ya - yb)x   +   (xb - xa)y + (xa * yb - xb * ya) = 0
		// Compute A = (ya - yb) for the 3 line segments that make up each triangle
		__m128i A0 = ssp_sub_epi32(fxPtY[1], fxPtY[2]);
		__m128i A1 = ssp_sub_epi32(fxPtY[2], fxPtY[0]);
		__m128i A2 = ssp_sub_epi32(fxPtY[0], fxPtY[1]);

		// Compute B = (xb - xa) for the 3 line segments that make up each triangle
		__m128i B0 = ssp_sub_epi32(fxPtX[2], fxPtX[1]);
		__m128i B1 = ssp_sub_epi32(fxPtX[0], fxPtX[2]);
		__m128i B2 = ssp_sub_epi32(fxPtX[1], fxPtX[0]);

		// Compute C = (xa * yb - xb * ya) for the 3 line segments that make up each triangle
		__m128i C0 = ssp_sub_epi32(ssp_mullo_epi32(fxPtX[1], fxPtY[2]), ssp_mullo_epi32(fxPtX[2], fxPtY[1]));
		__m128i C1 = ssp_sub_epi32(ssp_mullo_epi32(fxPtX[2], fxPtY[0]), ssp_mullo_epi32(fxPtX[0], fxPtY[2]));
		__m128i C2 = ssp_sub_epi32(ssp_mullo_epi32(fxPtX[0], fxPtY[1]), ssp_mullo_epi32(fxPtX[1], fxPtY[0]));

		// Compute triangle area
		__m128i triArea = ssp_mullo_epi32(B2, A1);
		triArea = ssp_sub_epi32(triArea, ssp_mullo_epi32(B1, A2));
		__m128 oneOverTriArea = ssp_div_ps(ssp_set1_ps(1.0f), ssp_cvtepi32_ps(triArea));

		__m128 Z[3];
		Z[0] = xformedPos[0].Z;
		Z[1] = ssp_mul_ps(ssp_sub_ps(xformedPos[1].Z, Z[0]), oneOverTriArea);
		Z[2] = ssp_mul_ps(ssp_sub_ps(xformedPos[2].Z, Z[0]), oneOverTriArea);
		
		// Use bounding box traversal strategy to determine which pixels to rasterize 
		__m128i startX =  ssp_and_si128(Max(Min(Min(fxPtX[0], fxPtX[1]), fxPtX[2]),  ssp_set1_epi32(0)), ssp_set1_epi32(~1));
		__m128i endX   = Min(Max(Max(fxPtX[0], fxPtX[1]), fxPtX[2]), ssp_set1_epi32(SCREENW - 1));

		__m128i startY = ssp_and_si128(Max(Min(Min(fxPtY[0], fxPtY[1]), fxPtY[2]), ssp_set1_epi32(0)), ssp_set1_epi32(~1));
		__m128i endY   = Min(Max(Max(fxPtY[0], fxPtY[1]), fxPtY[2]), ssp_set1_epi32(SCREENH - 1));

		// Now we have 4 triangles set up.  Rasterize them each individually.
        for(int lane=0; lane < SSE; lane++)
        {
			// Skip triangle if area is zero 
			if(triArea.m128i_i32[lane] <= 0)
			{
				continue;
			}

			// Extract this triangle's properties from the SIMD versions
            __m128 zz[3];
			for(int vv = 0; vv < 3; vv++)
			{
				zz[vv] = ssp_set1_ps(Z[vv].m128_f32[lane]);
			}

			int startXx = startX.m128i_i32[lane];
			int endXx	= endX.m128i_i32[lane];
			int startYy = startY.m128i_i32[lane];
			int endYy	= endY.m128i_i32[lane];
		
			__m128i aa0 = ssp_set1_epi32(A0.m128i_i32[lane]);
			__m128i aa1 = ssp_set1_epi32(A1.m128i_i32[lane]);
			__m128i aa2 = ssp_set1_epi32(A2.m128i_i32[lane]);

			__m128i bb0 = ssp_set1_epi32(B0.m128i_i32[lane]);
			__m128i bb1 = ssp_set1_epi32(B1.m128i_i32[lane]);
			__m128i bb2 = ssp_set1_epi32(B2.m128i_i32[lane]);

			__m128i aa0Inc = ssp_slli_epi32(aa0, 1);
			__m128i aa1Inc = ssp_slli_epi32(aa1, 1);
			__m128i aa2Inc = ssp_slli_epi32(aa2, 1);

			__m128i bb0Inc = ssp_slli_epi32(bb0, 1);
			__m128i bb1Inc = ssp_slli_epi32(bb1, 1);
			__m128i bb2Inc = ssp_slli_epi32(bb2, 1);

			__m128i row, col;

			// Tranverse pixels in 2x2 blocks and store 2x2 pixel quad depths contiguously in memory ==> 2*X
			// This method provides better perfromance
			int	rowIdx = (startYy * SCREENW + 2 * startXx);

			col = ssp_add_epi32(colOffset, ssp_set1_epi32(startXx));
			__m128i aa0Col = ssp_mullo_epi32(aa0, col);
			__m128i aa1Col = ssp_mullo_epi32(aa1, col);
			__m128i aa2Col = ssp_mullo_epi32(aa2, col);

			row = ssp_add_epi32(rowOffset, ssp_set1_epi32(startYy));
			__m128i bb0Row = ssp_add_epi32(ssp_mullo_epi32(bb0, row), ssp_set1_epi32(C0.m128i_i32[lane]));
			__m128i bb1Row = ssp_add_epi32(ssp_mullo_epi32(bb1, row), ssp_set1_epi32(C1.m128i_i32[lane]));
			__m128i bb2Row = ssp_add_epi32(ssp_mullo_epi32(bb2, row), ssp_set1_epi32(C2.m128i_i32[lane]));

			__m128i sum0Row = ssp_add_epi32(aa0Col, bb0Row);
			__m128i sum1Row = ssp_add_epi32(aa1Col, bb1Row);
			__m128i sum2Row = ssp_add_epi32(aa2Col, bb2Row);

			__m128 zx = ssp_mul_ps(ssp_cvtepi32_ps(aa1Inc), zz[1]);
			zx = ssp_add_ps(zx, ssp_mul_ps(ssp_cvtepi32_ps(aa2Inc), zz[2]));

			// Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY)
			for(int r = startYy; r < endYy; r += 2,
											rowIdx += 2 * SCREENW,
											sum0Row = ssp_add_epi32(sum0Row, bb0Inc),
											sum1Row = ssp_add_epi32(sum1Row, bb1Inc),
											sum2Row = ssp_add_epi32(sum2Row, bb2Inc))
			{
				// Compute barycentric coordinates 
				int index = rowIdx;
				__m128i alpha = sum0Row;
				__m128i beta = sum1Row;
				__m128i gama = sum2Row;

				//Compute barycentric-interpolated depth
				__m128 depth = zz[0];
				depth = ssp_add_ps(depth, ssp_mul_ps(ssp_cvtepi32_ps(beta), zz[1]));
				depth = ssp_add_ps(depth, ssp_mul_ps(ssp_cvtepi32_ps(gama), zz[2]));
				__m128i anyOut = ssp_setzero_si128();

				for(int c = startXx; c < endXx; c += 2,
												index += 4,
												alpha = ssp_add_epi32(alpha, aa0Inc),
												beta  = ssp_add_epi32(beta, aa1Inc),
												gama  = ssp_add_epi32(gama, aa2Inc),
												depth = ssp_add_ps(depth, zx))
				{
					//Test Pixel inside triangle
					__m128i mask = ssp_or_si128(ssp_or_si128(alpha, beta), gama);
					
					__m128 previousDepthValue = ssp_load_ps(&pDepthBuffer[index]);
					__m128 depthMask  = ssp_cmpge_ps(depth, previousDepthValue);
					__m128i finalMask = ssp_andnot_si128(mask, ssp_castps_si128(depthMask));
					anyOut = ssp_or_si128(anyOut, finalMask);
				}//for each column	
				
				if(!ssp_testz_si128(anyOut, ssp_set1_epi32(0x80000000)))
				{
					return true; //early exit
				}
			}// for each row
		}// for each triangle
	}// for each set of SIMD# triangles

	return false;
}