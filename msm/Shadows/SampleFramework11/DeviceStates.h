//=================================================================================================
//
//	MJP's DX11 Sample Framework
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

#pragma once

#include "PCH.h"

#include "InterfacePointers.h"

namespace SampleFramework11
{

class BlendStates
{

protected:

    ID3D11BlendStatePtr blendDisabled;
    ID3D11BlendStatePtr additiveBlend;
    ID3D11BlendStatePtr alphaBlend;
    ID3D11BlendStatePtr pmAlphaBlend;
    ID3D11BlendStatePtr noColor;
    ID3D11BlendStatePtr alphaToCoverage;
    ID3D11BlendStatePtr opacityBlend;

public:

    void Initialize(ID3D11Device* device);

    ID3D11BlendState* BlendDisabled () { return blendDisabled; };
    ID3D11BlendState* AdditiveBlend () { return additiveBlend; };
    ID3D11BlendState* AlphaBlend () { return alphaBlend; };
    ID3D11BlendState* PreMultipliedAlphaBlend () { return pmAlphaBlend; };
    ID3D11BlendState* ColorWriteDisabled () { return noColor; };
    ID3D11BlendState* AlphaToCoverage () { return alphaToCoverage; };
    ID3D11BlendState* OpacityBlend() { return opacityBlend; };

    static const D3D11_BLEND_DESC* BlendDisabledDesc();
    static const D3D11_BLEND_DESC* AdditiveBlendDesc();
    static const D3D11_BLEND_DESC* AlphaBlendDesc();
    static const D3D11_BLEND_DESC* PreMultipliedAlphaBlendDesc();
    static const D3D11_BLEND_DESC* ColorWriteDisabledDesc();
    static const D3D11_BLEND_DESC* AlphaToCoverageDesc();
    static const D3D11_BLEND_DESC* OpacityBlendDesc();
};


class RasterizerStates
{

protected:

    ID3D11RasterizerStatePtr noCull;
    ID3D11RasterizerStatePtr cullBackFaces;
    ID3D11RasterizerStatePtr cullBackFacesScissor;
    ID3D11RasterizerStatePtr cullFrontFaces;
    ID3D11RasterizerStatePtr cullFrontFacesScissor;
    ID3D11RasterizerStatePtr noCullNoMS;
    ID3D11RasterizerStatePtr noCullScissor;
    ID3D11RasterizerStatePtr wireframe;

public:

    void Initialize(ID3D11Device* device);

    ID3D11RasterizerState* NoCull() { return noCull; };
    ID3D11RasterizerState* BackFaceCull() { return cullBackFaces; };
    ID3D11RasterizerState* BackFaceCullScissor() { return cullBackFacesScissor; };
    ID3D11RasterizerState* FrontFaceCull() { return cullFrontFaces; };
    ID3D11RasterizerState* FrontFaceCullScissor() { return cullFrontFacesScissor; };
    ID3D11RasterizerState* NoCullNoMS() { return noCullNoMS; };
    ID3D11RasterizerState* NoCullScissor() { return noCullScissor; };
    ID3D11RasterizerState* Wireframe() { return wireframe; };

    static const D3D11_RASTERIZER_DESC* NoCullDesc();
    static const D3D11_RASTERIZER_DESC* FrontFaceCullDesc();
    static const D3D11_RASTERIZER_DESC* FrontFaceCullScissorDesc();
    static const D3D11_RASTERIZER_DESC* BackFaceCullDesc();
    static const D3D11_RASTERIZER_DESC* BackFaceCullScissorDesc();
    static const D3D11_RASTERIZER_DESC* NoCullNoMSDesc();
    static const D3D11_RASTERIZER_DESC* NoCullScissorDesc();
    static const D3D11_RASTERIZER_DESC* WireframeDesc();
};


class DepthStencilStates
{
    ID3D11DepthStencilStatePtr depthDisabled;
    ID3D11DepthStencilStatePtr depthEnabled;
    ID3D11DepthStencilStatePtr revDepthEnabled;
    ID3D11DepthStencilStatePtr depthWriteEnabled;
    ID3D11DepthStencilStatePtr revDepthWriteEnabled;
    ID3D11DepthStencilStatePtr depthStencilWriteEnabled;
    ID3D11DepthStencilStatePtr stencilEnabled;

public:

    void Initialize(ID3D11Device* device);

    ID3D11DepthStencilState* DepthDisabled() { return depthDisabled; };
    ID3D11DepthStencilState* DepthEnabled() { return depthEnabled; };
    ID3D11DepthStencilState* ReverseDepthEnabled() { return revDepthEnabled; };
    ID3D11DepthStencilState* DepthWriteEnabled() { return depthWriteEnabled; };
    ID3D11DepthStencilState* ReverseDepthWriteEnabled() { return revDepthWriteEnabled; };
    ID3D11DepthStencilState* DepthStencilWriteEnabled() { return depthStencilWriteEnabled; };
    ID3D11DepthStencilState* StencilTestEnabled() { return depthStencilWriteEnabled; };

    static const D3D11_DEPTH_STENCIL_DESC* DepthDisabledDesc();
    static const D3D11_DEPTH_STENCIL_DESC* DepthEnabledDesc();
    static const D3D11_DEPTH_STENCIL_DESC* ReverseDepthEnabledDesc();
    static const D3D11_DEPTH_STENCIL_DESC* DepthWriteEnabledDesc();
    static const D3D11_DEPTH_STENCIL_DESC* ReverseDepthWriteEnabledDesc();
    static const D3D11_DEPTH_STENCIL_DESC* DepthStencilWriteEnabledDesc();
    static const D3D11_DEPTH_STENCIL_DESC* StencilEnabledDesc();
};


class SamplerStates
{

    ID3D11SamplerStatePtr linear;
    ID3D11SamplerStatePtr linearClamp;
    ID3D11SamplerStatePtr linearBorder;
    ID3D11SamplerStatePtr point;
    ID3D11SamplerStatePtr anisotropic;
    ID3D11SamplerStatePtr shadowMap;
    ID3D11SamplerStatePtr shadowMapPCF;
public:

    void Initialize(ID3D11Device* device);

    ID3D11SamplerState* Linear() { return linear; };
    ID3D11SamplerState* LinearClamp() { return linearClamp; };
    ID3D11SamplerState* LinearBorder() { return linearBorder; };
    ID3D11SamplerState* Point() { return point; };
    ID3D11SamplerState* Anisotropic() { return anisotropic; };
    ID3D11SamplerState* ShadowMap() { return shadowMap; };
    ID3D11SamplerState* ShadowMapPCF() { return shadowMapPCF; };

    static const D3D11_SAMPLER_DESC* LinearDesc();
    static const D3D11_SAMPLER_DESC* LinearClampDesc();
    static const D3D11_SAMPLER_DESC* LinearBorderDesc();
    static const D3D11_SAMPLER_DESC* PointDesc();
    static const D3D11_SAMPLER_DESC* AnisotropicDesc();
    static const D3D11_SAMPLER_DESC* ShadowMapDesc();
    static const D3D11_SAMPLER_DESC* ShadowMapPCFDesc();
};

}
