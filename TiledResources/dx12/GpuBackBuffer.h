#pragma once

#include "GpuResource.h"
#include "GpuPixelBuffer.h"
#include "GpuDescriptorHeap.h"

namespace TiledResources
{
    class GpuBackBuffer : public GpuPixelBuffer
    {

    private:
        using Base = GpuPixelBuffer;

    public:
        GpuBackBuffer() : Base(nullptr)
        {

        }

        GpuBackBuffer(ID3D12Resource* resource) : Base(nullptr)
        {

        }

        GpuBackBuffer(ID3D12Resource* resource, DescriptorHandle rtv) : Base(resource)
            , m_RTV(rtv)
        {

        }

        DescriptorHandle   RTV() const
        {
            return m_RTV;
        }

    private:
        DescriptorHandle   m_RTV;
    };
}
