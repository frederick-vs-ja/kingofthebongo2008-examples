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

#include "TransformedMeshSSE.h"

TransformedMeshSSE::TransformedMeshSSE()
	: mNumVertices(0),
	  mNumIndices(0),
	  mNumTriangles(0),
	  mpVertices(NULL),
	  mpIndices(NULL)
{
	mpXformedPos[0] = mpXformedPos[1] = NULL;
}

TransformedMeshSSE::~TransformedMeshSSE()
{

}

void TransformedMeshSSE::Initialize(CPUTMeshDX11* pMesh)
{
	mNumVertices = pMesh->GetVertexCount();
	mNumIndices  = pMesh->GetIndexCount();
	mNumTriangles = pMesh->GetTriangleCount();
	mpVertices   = pMesh->GetVertices();
	mpIndices    = pMesh->GetIndices();
}

//-------------------------------------------------------------------
// Trasforms the occluder vertices to screen space once every frame
//-------------------------------------------------------------------
void TransformedMeshSSE::TransformVertices(__m128 *cumulativeMatrix, 
										   UINT start, 
										   UINT end,
										   UINT idx)
{
	UINT i;
	for(i = start; i <= end; i++)
	{
		__m128 xform = TransformCoords(&mpVertices[i].position, cumulativeMatrix);
		__m128 vertZ = ssp_shuffle_ps(xform, xform, 0xaa);
		__m128 vertW = ssp_shuffle_ps(xform, xform, 0xff);
		__m128 projected = ssp_div_ps(xform, vertW);

		//set to all 0s if clipped by near clip plane
		__m128 noNearClip = ssp_cmple_ps(vertZ, vertW);
		mpXformedPos[idx][i] = ssp_and_ps(projected, noNearClip);
	}
}

void TransformedMeshSSE::Gather(vFloat4 pOut[3], UINT triId, UINT numLanes, UINT idx)
{
	const UINT *pInd0 = &mpIndices[triId * 3];
	const UINT *pInd1 = pInd0 + (numLanes > 1 ? 3 : 0);
	const UINT *pInd2 = pInd0 + (numLanes > 2 ? 6 : 0);
	const UINT *pInd3 = pInd0 + (numLanes > 3 ? 9 : 0);

	for(UINT i = 0; i < 3; i++)
	{
		__m128 v0 = mpXformedPos[idx][pInd0[i]];	// x0 y0 z0 w0
		__m128 v1 = mpXformedPos[idx][pInd1[i]];	// x1 y1 z1 w1
		__m128 v2 = mpXformedPos[idx][pInd2[i]];	// x2 y2 z2 w2
		__m128 v3 = mpXformedPos[idx][pInd3[i]];	// x3 y3 z3 w3
		_MM_TRANSPOSE4_PS(v0, v1, v2, v3);
		pOut[i].X = v0;
		pOut[i].Y = v1;
		pOut[i].Z = v2;
		pOut[i].W = v3;
	}
}

//--------------------------------------------------------------------------------
// Bin the screen space transformed triangles into tiles. For single threaded version
//--------------------------------------------------------------------------------
void TransformedMeshSSE::BinTransformedTrianglesST(UINT taskId,
												   UINT modelId,
												   UINT meshId,
												   UINT start,
												   UINT end,
												   UINT* pBin,
												   USHORT* pBinModel,
												   USHORT* pBinMesh,
												   USHORT* pNumTrisInBin,
												   UINT idx)
{
	int numLanes = SSE;
	int laneMask = (1 << numLanes) - 1; 
	// working on 4 triangles at a time
	for(UINT index = start; index <= end; index += SSE)
	{
		if(index + SSE > end)
		{
			numLanes = end - index + 1;
			laneMask = (1 << numLanes) - 1; 
		}
		
		// storing x,y,z,w for the 3 vertices of 4 triangles = 4*3*4 = 48
		vFloat4 xformedPos[3];		
		Gather(xformedPos, index, numLanes, idx);
		
		// TODO: Maybe convert to Fixed pt and store it once so that dont have to convert to fixedPt again during rasterization
		__m128i fxPtX[3], fxPtY[3];
		for(int i = 0; i < 3; i++)
		{
			fxPtX[i] = ssp_cvtps_epi32(xformedPos[i].X);
			fxPtY[i] = ssp_cvtps_epi32(xformedPos[i].Y);
		}

		// Compute triangle are
		__m128i triArea1 = ssp_sub_epi32(fxPtX[1], fxPtX[0]);
		triArea1 = ssp_mullo_epi32(triArea1, ssp_sub_epi32(fxPtY[2], fxPtY[0]));

		__m128i triArea2 = ssp_sub_epi32(fxPtX[0], fxPtX[2]);
		triArea2 = ssp_mullo_epi32(triArea2, ssp_sub_epi32(fxPtY[0], fxPtY[1]));

		__m128i triArea = ssp_sub_epi32(triArea1, triArea2);
				
		// Find bounding box for screen space triangle in terms of pixels
		__m128i vStartX = Max(Min(Min(fxPtX[0], fxPtX[1]), fxPtX[2]), ssp_set1_epi32(0));
		__m128i vEndX   = Min(Max(Max(fxPtX[0], fxPtX[1]), fxPtX[2]), ssp_set1_epi32(SCREENW - 1));

        __m128i vStartY = Max(Min(Min(fxPtY[0], fxPtY[1]), fxPtY[2]), ssp_set1_epi32(0));
        __m128i vEndY   = Min(Max(Max(fxPtY[0], fxPtY[1]), fxPtY[2]), ssp_set1_epi32(SCREENH - 1));

		//Figure out which lanes are active
		__m128i front = ssp_cmpgt_epi32(triArea, ssp_setzero_si128());
		__m128i nonEmptyX = ssp_cmpgt_epi32(vEndX, vStartX);
		__m128i nonEmptyY = ssp_cmpgt_epi32(vEndY, vStartY);
		__m128 accept1 = ssp_castsi128_ps(ssp_and_si128(ssp_and_si128(front, nonEmptyX), nonEmptyY));

		// All verts must be inside the near clip volume
		__m128 W0 = ssp_cmpgt_ps(xformedPos[0].W, ssp_setzero_ps());
		__m128 W1 = ssp_cmpgt_ps(xformedPos[1].W, ssp_setzero_ps());
		__m128 W2 = ssp_cmpgt_ps(xformedPos[2].W, ssp_setzero_ps());

		__m128 accept = ssp_and_ps(ssp_and_ps(accept1, W0), ssp_and_ps(W1, W2));
		unsigned int triMask = ssp_movemask_ps(accept) & laneMask; 
		
		while(triMask)
		{
			int i = FindClearLSB(&triMask);
			// Convert bounding box in terms of pixels to bounding box in terms of tiles
			int startX = max(vStartX.m128i_i32[i]/TILE_WIDTH_IN_PIXELS, 0);
			int endX   = min(vEndX.m128i_i32[i]/TILE_WIDTH_IN_PIXELS, SCREENW_IN_TILES-1);

			int startY = max(vStartY.m128i_i32[i]/TILE_HEIGHT_IN_PIXELS, 0);
			int endY   = min(vEndY.m128i_i32[i]/TILE_HEIGHT_IN_PIXELS, SCREENH_IN_TILES-1);

			// Add triangle to the tiles or bins that the bounding box covers
			int row, col;
			for(row = startY; row <= endY; row++)
			{
				int offset1 = YOFFSET1_ST * row;
				int offset2 = YOFFSET2_ST * row;
				for(col = startX; col <= endX; col++)
				{
					int idx1 = offset1 + (XOFFSET1_ST * col) + taskId;
					int idx2 = offset2 + (XOFFSET2_ST * col) + (taskId * MAX_TRIS_IN_BIN_ST) + pNumTrisInBin[idx1];
					pBin[idx2] = index + i;
					pBinModel[idx2] = modelId;
					pBinMesh[idx2] = meshId;
					pNumTrisInBin[idx1] += 1;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------
// Bin the screen space transformed triangles into tiles. For multi threaded version
//--------------------------------------------------------------------------------
void TransformedMeshSSE::BinTransformedTrianglesMT(UINT taskId,
												   UINT modelId,
												   UINT meshId,
												   UINT start,
												   UINT end,
												   UINT* pBin,
												   USHORT* pBinModel,
												   USHORT* pBinMesh,
												   USHORT* pNumTrisInBin,
												   UINT idx)
{
	int numLanes = SSE;
	int laneMask = (1 << numLanes) - 1; 
	// working on 4 triangles at a time
	for(UINT index = start; index <= end; index += SSE)
	{
		if(index + SSE > end)
		{
			numLanes = end - index + 1;
			laneMask = (1 << numLanes) - 1; 
		}
		
		// storing x,y,z,w for the 3 vertices of 4 triangles = 4*3*4 = 48
		vFloat4 xformedPos[3];
		Gather(xformedPos, index, numLanes, idx);
			
		// TODO: Maybe convert to Fixed pt and store it once so that dont have to convert to fixedPt again during rasterization
		__m128i fxPtX[3], fxPtY[3];	
		for(int i = 0; i < 3; i++)
		{
			fxPtX[i] = ssp_cvtps_epi32(xformedPos[i].X);
			fxPtY[i] = ssp_cvtps_epi32(xformedPos[i].Y);		
		}

		__m128i triArea1 = ssp_sub_epi32(fxPtX[1], fxPtX[0]);
		triArea1 = ssp_mullo_epi32(triArea1, ssp_sub_epi32(fxPtY[2], fxPtY[0]));

		__m128i triArea2 = ssp_sub_epi32(fxPtX[0], fxPtX[2]);
		triArea2 = ssp_mullo_epi32(triArea2, ssp_sub_epi32(fxPtY[0], fxPtY[1]));

		__m128i triArea = ssp_sub_epi32(triArea1, triArea2);
		
		// Find bounding box for screen space triangle in terms of pixels
		__m128i vStartX = Max(Min(Min(fxPtX[0], fxPtX[1]), fxPtX[2]), ssp_set1_epi32(0));
		__m128i vEndX   = Min(Max(Max(fxPtX[0], fxPtX[1]), fxPtX[2]), ssp_set1_epi32(SCREENW - 1));

        __m128i vStartY = Max(Min(Min(fxPtY[0], fxPtY[1]), fxPtY[2]), ssp_set1_epi32(0));
        __m128i vEndY   = Min(Max(Max(fxPtY[0], fxPtY[1]), fxPtY[2]),  ssp_set1_epi32(SCREENH -1));

		//Figure out which lanes are active
		__m128i front = ssp_cmpgt_epi32(triArea, ssp_setzero_si128());
		__m128i nonEmptyX = ssp_cmpgt_epi32(vEndX, vStartX);
		__m128i nonEmptyY = ssp_cmpgt_epi32(vEndY, vStartY);
		__m128 accept1 = ssp_castsi128_ps(ssp_and_si128(ssp_and_si128(front, nonEmptyX), nonEmptyY));

		// All verts must be inside the near clip volume
		__m128 W0 = ssp_cmpgt_ps(xformedPos[0].W, ssp_setzero_ps());
		__m128 W1 = ssp_cmpgt_ps(xformedPos[1].W, ssp_setzero_ps());
		__m128 W2 = ssp_cmpgt_ps(xformedPos[2].W, ssp_setzero_ps());

		__m128 accept = ssp_and_ps(ssp_and_ps(accept1, W0), ssp_and_ps(W1, W2));
		unsigned int triMask = ssp_movemask_ps(accept) & laneMask; 
			
		while(triMask)
		{
			int i = FindClearLSB(&triMask);
				
			// Convert bounding box in terms of pixels to bounding box in terms of tiles
			int startX = max(vStartX.m128i_i32[i]/TILE_WIDTH_IN_PIXELS, 0);
			int endX   = min(vEndX.m128i_i32[i]/TILE_WIDTH_IN_PIXELS, SCREENW_IN_TILES-1);
			int startY = max(vStartY.m128i_i32[i]/TILE_HEIGHT_IN_PIXELS, 0);
			int endY   = min(vEndY.m128i_i32[i]/TILE_HEIGHT_IN_PIXELS, SCREENH_IN_TILES-1);

			// Add triangle to the tiles or bins that the bounding box covers
			int row, col;
			for(row = startY; row <= endY; row++)
			{
				int offset1 = YOFFSET1_MT * row;
				int offset2 = YOFFSET2_MT * row;
				for(col = startX; col <= endX; col++)
				{
					int idx1 = offset1 + (XOFFSET1_MT * col) + (TOFFSET1_MT * taskId);
					int idx2 = offset2 + (XOFFSET2_MT * col) + (taskId * MAX_TRIS_IN_BIN_MT) + pNumTrisInBin[idx1];
					pBin[idx2] = index + i;
					pBinModel[idx2] = modelId;
					pBinMesh[idx2] = meshId;
					pNumTrisInBin[idx1] += 1;
				}
			}
		}
	}
}


void TransformedMeshSSE::GetOneTriangleData(__m128 xformedPos[3], UINT triId, UINT idx)
{
	const UINT *inds = &mpIndices[triId * 3];
	for(int i = 0; i < 3; i++)
	{
		xformedPos[i] = mpXformedPos[idx][inds[i]];
	}
}