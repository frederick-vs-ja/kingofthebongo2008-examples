//=================================================================================================
//
//	Shadows Sample
//  by MJP
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

// ================================================================================================
// Constant buffers
// ================================================================================================
cbuffer VSConstants : register(b0)
{
    float4x4 World;
    float4x4 ViewProjection;
}

// ================================================================================================
// Vertex Shader
// ================================================================================================
float4 VS(in float3 PositionOS : POSITION) : SV_Position
{
    float3 positionWS = mul(float4(PositionOS, 1.0f), World).xyz;
    return mul(float4(positionWS, 1.0f), ViewProjection);
}