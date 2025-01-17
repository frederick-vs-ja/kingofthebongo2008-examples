//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED ARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "DirectXHelper.h"
#include "SampleSettings.h"
#include "ResidencyManager.h"

using namespace TiledResources;

using namespace concurrency;
using namespace DirectX;
using namespace Windows::Foundation;

unsigned g_frame = 0;

ResidencyManager::ResidencyManager(const std::shared_ptr<DeviceResources>& deviceResources) :
{
    int temp = 0;

    /*
    m_deviceResources(deviceResources),
        m_debugMode(true),
        m_activeTileLoadingOperations(0),
        m_reservedTiles(0),
        m_defaultTileIndex(-1)
    CreateDeviceDependentResources();
    */
}

void ResidencyManager::CreateDeviceDependentResources()
{
    /*
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Create the constant buffer for viewer constants.
    D3D11_BUFFER_DESC constantBufferDesc = {};
    
    constantBufferDesc.ByteWidth = 16;
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    DX::ThrowIfFailed(device->CreateBuffer(&constantBufferDesc, nullptr, &m_viewerVertexShaderConstantBuffer));

    // Create the viewer vertex buffer.
    float vertexBufferData[] = 
    {
        -1.0f, 0.5f, 1.0f, -1.0f, 1.0f, 0.0f,
        -0.5f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, -1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 0.5f, 1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, -0.5f, 1.0f, -1.0f, -1.0f, 0.0f,
        0.5f, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f,
        0.0f, -0.5f, -1.0f, 1.0f, -1.0f, 0.0f,
        -0.5f, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f,
        -1.0f, -0.5f, 1.0f, -1.0f, -1.0f, 0.0f,
        -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f
    };
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    
    vertexBufferDesc.ByteWidth = sizeof(vertexBufferData);
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vertexBufferInitialData = {vertexBufferData, 0, 0};
    DX::ThrowIfFailed(device->CreateBuffer(&vertexBufferDesc, &vertexBufferInitialData, &m_viewerVertexBuffer));

    // Create the viewer index buffer.
    unsigned int indexBufferData[] = 
    {
        0,1,10,
        1,2,10,
        2,7,10,
        7,8,10,
        8,9,10,
        9,0,10,
        2,3,11,
        3,4,11,
        4,5,11,
        5,6,11,
        6,7,11,
        7,2,11
    };
    D3D11_BUFFER_DESC indexBufferDesc = {};
    
    indexBufferDesc.ByteWidth = sizeof(indexBufferData);
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA indexBufferInitialData = {indexBufferData, 0, 0};
    DX::ThrowIfFailed(device->CreateBuffer(&indexBufferDesc, &indexBufferInitialData, &m_viewerIndexBuffer));
    m_indexCount = ARRAYSIZE(indexBufferData);

    // Create wrapping point sampler.
    D3D11_SAMPLER_DESC samplerDesc = {};
    
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    DX::ThrowIfFailed(device->CreateSamplerState(&samplerDesc, &m_sampler));

    // Create the tile pool.
    D3D11_BUFFER_DESC tilePoolDesc = {};
    
    tilePoolDesc.ByteWidth = SampleSettings::TileSizeInBytes * SampleSettings::TileResidency::PoolSizeInTiles;
    tilePoolDesc.Usage = D3D11_USAGE_DEFAULT;
    tilePoolDesc.MiscFlags = D3D11_RESOURCE_MISC_TILE_POOL;
    DX::ThrowIfFailed(device->CreateBuffer(&tilePoolDesc, nullptr, &m_tilePool));

    {
        D3D11_DEPTH_STENCIL_DESC dss = {};
        dss.DepthEnable = true;
        dss.DepthFunc = D3D11_COMPARISON_LESS;
        dss.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        DX::ThrowIfFailed(device->CreateDepthStencilState(&dss, &m_viewerState));
    }

    {
        D3D11_DEPTH_STENCIL_DESC dss = {};
        DX::ThrowIfFailed(device->CreateDepthStencilState(&dss, &m_viewerDisabledState));
    }
    */
}

task<void> ResidencyManager::CreateDeviceDependentResourcesAsync()
{
    /*
    // Load and create the vertex shader and input layout.
    auto vsTask = DX::ReadDataAsync(L"ResidencyViewer.vs.cso").then([this](std::vector<byte> fileData)
    {
        auto device = m_deviceResources->GetD3DDevice();
        D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = 
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        DX::ThrowIfFailed(device->CreateInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), fileData.data(), fileData.size(), &m_viewerInputLayout));
        DX::ThrowIfFailed(device->CreateVertexShader(fileData.data(), fileData.size(), nullptr, &m_viewerVertexShader));
    });

    // Load and create the pixel shader.
    auto psTask = DX::ReadDataAsync(L"ResidencyViewer.ps.cso").then([this](std::vector<byte> fileData)
    {
        auto device = m_deviceResources->GetD3DDevice();
        DX::ThrowIfFailed(device->CreatePixelShader(fileData.data(), fileData.size(), nullptr, &m_viewerPixelShader));
    });

    return (vsTask && psTask);
    */
}

void ResidencyManager::ReleaseDeviceDependentResources()
{
    /*
    m_viewerVertexBuffer.Reset();
    m_viewerIndexBuffer.Reset();
    m_viewerInputLayout.Reset();
    m_viewerVertexShader.Reset();
    m_viewerPixelShader.Reset();
    m_sampler.Reset();
    m_viewerVertexShaderConstantBuffer.Reset();
    m_tilePool.Reset();

    m_managedResources.clear();
    m_trackedTiles.clear();
    m_seenTileList.clear();
    m_loadingTileList.clear();
    m_mappedTileList.clear();
    */
}

void ResidencyManager::RenderVisualization()
{
    /*
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Set up pipeline state for rendering the visualization.
    auto targetView = m_deviceResources->GetBackBufferRenderTargetView();
    context->OMSetRenderTargets(1, &targetView, nullptr);
    UINT stride = sizeof(float) * 6;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_viewerVertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetInputLayout(m_viewerInputLayout.Get());
    context->IASetIndexBuffer(m_viewerIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_viewerVertexShader.Get(), nullptr, 0);
    context->VSSetConstantBuffers(0, 1, m_viewerVertexShaderConstantBuffer.GetAddressOf());
    context->PSSetShader(m_viewerPixelShader.Get(), nullptr, 0);
    context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
    context->OMSetDepthStencilState(m_viewerDisabledState.Get(), 0);
    
    // Render each resource's residency map in its own visualizer.
    float yOffset = 0;
    for (auto & managedResource : m_managedResources)
    {
        // Update the constant buffer to position the visualization.
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        DX::ThrowIfFailed(context->Map(m_viewerVertexShaderConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
        const float visualWidth = max(256.0f, m_deviceResources->GetScreenViewport().Width / SampleSettings::Sampling::Ratio);
        const float visualHeight = visualWidth * sqrtf(3.0f) / 3.0f;
        const float xPosition = m_deviceResources->GetScreenViewport().Width - visualWidth - 48.0f;
        const float yPosition = m_deviceResources->GetScreenViewport().Height - visualHeight - 48.0f - yOffset;
        static_cast<float*>(mappedResource.pData)[0] = visualWidth / m_deviceResources->GetScreenViewport().Width;
        static_cast<float*>(mappedResource.pData)[1] = visualHeight / m_deviceResources->GetScreenViewport().Height;
        static_cast<float*>(mappedResource.pData)[2] = 2.0f * (xPosition + visualWidth / 2.0f) / m_deviceResources->GetScreenViewport().Width - 1.0f;
        static_cast<float*>(mappedResource.pData)[3] = 1.0f - 2.0f * (yPosition + visualHeight / 2.0f) / m_deviceResources->GetScreenViewport().Height;
        context->Unmap(m_viewerVertexShaderConstantBuffer.Get(), 0);
        yOffset += visualHeight + 24.0f;

        // Bind the appropriate residency map.
        context->PSSetShaderResources(0, 1, managedResource->residencyTextureView.GetAddressOf());

        // Render the visualization.
        context->DrawIndexed(m_indexCount, 0, 0);
    }
    */
}

void ResidencyManager::EnqueueSamples(const std::vector<DecodedSample>& samples)
{
    /*
    for (auto sample : samples)
    {
        // Interpret each sample in the context of each managed resource.
        for (auto managedResource : m_managedResources)
        {
            // Samples are encoded assuming a maximally-sized (15-MIP) texture, so offset the
            // sampled value by the difference between actual MIPs and maximum MIPs.
            // TODO - MipLevels here (now just "15" because I'm using maximum for both layers) needs to refer to the full mipchain count, otherwise e.g. the maximally sized normal map mip value would still get offset
            short actualMip =
                managedResource->textureDesc.Format == DXGI_FORMAT_BC1_UNORM ?
                max(0, min(SampleSettings::TerrainAssets::Diffuse::UnpackedMipCount - 1, sample.mip)) :
                max(0, min(SampleSettings::TerrainAssets::Normal::UnpackedMipCount - 1, sample.mip));

            // Loop over sampled MIP through the least detailed MIP.
            for (short mip = actualMip; mip < static_cast<short>(managedResource->textureDesc.MipLevels); mip++)
            {
                // Calculate the tile coordinate.
                TileKey tileKey = {};
                tileKey.resource = managedResource->texture;
                tileKey.coordinate.Subresource = mip + sample.face * managedResource->textureDesc.MipLevels;
                float tileX = managedResource->subresourceTilings[tileKey.coordinate.Subresource].WidthInTiles * sample.u;
                tileX = min(managedResource->subresourceTilings[tileKey.coordinate.Subresource].WidthInTiles - 1, max(0, tileX));
                tileKey.coordinate.X = static_cast<UINT>(tileX);
                float tileY = managedResource->subresourceTilings[tileKey.coordinate.Subresource].HeightInTiles * sample.v;
                tileY = min(managedResource->subresourceTilings[tileKey.coordinate.Subresource].HeightInTiles - 1, max(0, tileY));
                tileKey.coordinate.Y = static_cast<UINT>(tileY);

                std::ostringstream tileInfoMessage;
                if (m_debugMode)
                {
                    tileInfoMessage << "ResidencyManager::EnqueueSamples : Looking For Tile : Face = ";
                    switch(sample.face)
                    {
                    case 0:
                        tileInfoMessage << "[+X]";
                        break;
                    case 1:
                        tileInfoMessage << "[-X]";
                        break;
                    case 2:
                        tileInfoMessage << "[+Y]";
                        break;
                    case 3:
                        tileInfoMessage << "[-Y]";
                        break;
                    case 4:
                        tileInfoMessage << "[+Z]";
                        break;
                    case 5:
                        tileInfoMessage << "[-Z]";
                        break;
                    default:
                        tileInfoMessage << "[error]";
                        break;
                    }
                    tileInfoMessage << " MIP = [" << mip << "]";
                    tileInfoMessage << " UV = (" << sample.u << "," << sample.v << ")";
                    tileInfoMessage << " Coordinate = [" << tileKey.coordinate.Subresource << "][" << tileKey.coordinate.Y << "][" << tileKey.coordinate.X << "]";
                }

                // See if the tile is already being tracked.
                auto tileIterator = m_trackedTiles.find(tileKey);
                if (tileIterator == m_trackedTiles.end())
                {
                    // Tile is not being tracked currently, so enqueue it for load.
                    if (m_debugMode)
                    {
                        tileInfoMessage << " --> Not Found : Enqueueing new tile.";
                    }
                    std::shared_ptr<TrackedTile> tile(new TrackedTile);
                    tile->managedResource = managedResource.get();
                    tile->coordinate = tileKey.coordinate;
                    tile->lastSeen = g_frame;
                    tile->state = TileState::Seen;
                    tile->mipLevel = mip;
                    tile->face = sample.face;
                    m_trackedTiles[tileKey] = tile;
                    m_seenTileList.push_back(tile);
                }
                else
                {
                    // If tile is already tracked, simply update the last-seen value.
                    if (m_debugMode)
                    {
                        tileInfoMessage << " --> Found in";
                        switch(tileIterator->second->state)
                        {
                        case TileState::Seen:
                            tileInfoMessage << " Seen list.";
                            break;
                        case TileState::Loading:
                        case TileState::Loaded:
                            tileInfoMessage << " Loading list.";
                            break;
                        case TileState::Mapped:
                            tileInfoMessage << " Mapped list.";
                            break;
                        default:
                            tileInfoMessage << " UNKNOWN STATE.";
                            break;
                        }
                    }
                    tileIterator->second->lastSeen = g_frame;
                }
#ifdef _DEBUG
                if (m_debugMode)
                {
                    tileInfoMessage << std::endl;
                    OutputDebugStringA(tileInfoMessage.str().c_str());
                }
#endif
            }
        }
    }
    g_frame++;
    */
}

void ResidencyManager::ProcessQueues()
{
    /*
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Sort the tile lists.
    m_seenTileList.sort(LoadPredicate);
    m_loadingTileList.sort(MapPredicate);
    m_mappedTileList.sort(EvictPredicate);

    // Initiate loads for seen tiles.
    for (int i = m_activeTileLoadingOperations; i < SampleSettings::TileResidency::MaxSimultaneousFileLoadTasks; i++)
    {
        if (m_seenTileList.empty())
        {
            break;
        }
        auto tileToLoad = m_seenTileList.front();
        m_seenTileList.pop_front();

        InterlockedIncrement(&m_activeTileLoadingOperations);
        tileToLoad->managedResource->loader->LoadTileAsync(tileToLoad->coordinate).then([this, tileToLoad](std::vector<byte> tileData)
        {
            tileToLoad->tileData = tileData;
            tileToLoad->state = TileState::Loaded;
            InterlockedDecrement(&m_activeTileLoadingOperations);
        });

        // Move the tile to the loading list.
        m_loadingTileList.push_back(tileToLoad);
    }

    // Loop over the loading / loaded tile list for mapping candidates.
    struct TileMappingUpdateArguments
    {
        std::vector<D3D11_TILED_RESOURCE_COORDINATE> coordinates;
        std::vector<UINT> rangeFlags;
        std::vector<UINT> physicalOffsets;
        // For convenience, the tracked tile mapping is also saved.
        std::list<std::shared_ptr<TrackedTile>> tilesToMap;
    };
    std::map<ID3D11Texture2D*, TileMappingUpdateArguments> coalescedMapArguments;

    for (int i = 0; i < SampleSettings::TileResidency::MaxTilesLoadedPerFrame; i++)
    {
        if (m_loadingTileList.empty())
        {
            break;
        }

        auto tileToMap = m_loadingTileList.front();
        if (tileToMap->state != TileState::Loaded)
        {
            // This sample's residency management assumes that for a given texcoord,
            // there will never be a detailed MIP resident where a less detailed one
            // is NULL-mapped. This is enforced by sort predicates. A side-effect of
            // this technique is that mapping cannot occur out of order.
            break;
        }
        m_loadingTileList.pop_front();
        
        // Default to assigning tiles to the first available tile.
        UINT physicalTileOffset = m_reservedTiles + static_cast<UINT>(m_mappedTileList.size());

        if (m_mappedTileList.size() + m_reservedTiles == SampleSettings::TileResidency::PoolSizeInTiles)
        {
            // Tile pool is full, need to unmap something.
            auto tileToEvict = m_mappedTileList.front();

            if (tileToMap->lastSeen < tileToEvict->lastSeen)
            {
                // If the candidate tile to map is older than the eviction candidate,
                // skip the mapping and discard it. This can occur if a tile load stalls,
                // and by the time it is ready it has moved off-screen.
                TileKey tileKey;
                tileKey.coordinate = tileToMap->coordinate;
                tileKey.resource = tileToMap->managedResource->texture;

                // Remove the tile from the tracked list.
                m_trackedTiles.erase(tileKey);

                // Move on to the next map candidate.
                continue;
            }

            m_mappedTileList.pop_front();
            TileKey tileKey;
            tileKey.coordinate = tileToEvict->coordinate;
            tileKey.resource = tileToEvict->managedResource->texture;

            // Remove the tile from the tracked list.
            m_trackedTiles.erase(tileKey);

            // Save the physical tile that was freed so the new tile can use it.
            physicalTileOffset = tileToEvict->physicalTileOffset;

            // Add the new NULL-mapping to the argument list.
            coalescedMapArguments[tileToEvict->managedResource->texture].coordinates.push_back(tileToEvict->coordinate);
            //coalescedMapArguments[tileToEvict->managedResource->texture].rangeFlags.push_back(D3D11_TILE_RANGE_NULL);
            //coalescedMapArguments[tileToEvict->managedResource->texture].physicalOffsets.push_back(physicalTileOffset);

            coalescedMapArguments[tileToEvict->managedResource->texture].rangeFlags.push_back(D3D11_TILE_RANGE_REUSE_SINGLE_TILE);
            coalescedMapArguments[tileToEvict->managedResource->texture].physicalOffsets.push_back(m_defaultTileIndex);

            // Update the residency map to remove this level of detail.
            int baseTilesCoveredWidth = tileToEvict->managedResource->subresourceTilings[0].WidthInTiles / tileToEvict->managedResource->subresourceTilings[tileToEvict->coordinate.Subresource].WidthInTiles;
            int baseTilesCoveredHeight = tileToEvict->managedResource->subresourceTilings[0].HeightInTiles / tileToEvict->managedResource->subresourceTilings[tileToEvict->coordinate.Subresource].HeightInTiles;
            for (int Y = 0; Y < baseTilesCoveredHeight; Y++)
            {
                for (int X = 0; X < baseTilesCoveredWidth; X++)
                {
                    int tileY = tileToEvict->coordinate.Y * baseTilesCoveredHeight + Y;
                    int tileX = tileToEvict->coordinate.X * baseTilesCoveredWidth + X;
                    byte* value = &tileToEvict->managedResource->residency[tileToEvict->face][tileY * tileToEvict->managedResource->subresourceTilings[0].WidthInTiles + tileX];
                    *value = max((tileToEvict->mipLevel + 1) * 16, *value);
                }
            }
        }

        // Add the new mapping to the argument list.
        coalescedMapArguments[tileToMap->managedResource->texture].coordinates.push_back(tileToMap->coordinate);
        coalescedMapArguments[tileToMap->managedResource->texture].rangeFlags.push_back(0);
        coalescedMapArguments[tileToMap->managedResource->texture].physicalOffsets.push_back(physicalTileOffset);
        tileToMap->physicalTileOffset = physicalTileOffset;
        tileToMap->state = TileState::Mapped;
        coalescedMapArguments[tileToMap->managedResource->texture].tilesToMap.push_back(tileToMap);

        // Update the residency map to add this level of detail.
        int baseTilesCoveredWidth = tileToMap->managedResource->subresourceTilings[0].WidthInTiles / tileToMap->managedResource->subresourceTilings[tileToMap->coordinate.Subresource].WidthInTiles;
        int baseTilesCoveredHeight = tileToMap->managedResource->subresourceTilings[0].HeightInTiles / tileToMap->managedResource->subresourceTilings[tileToMap->coordinate.Subresource].HeightInTiles;
        for (int Y = 0; Y < baseTilesCoveredHeight; Y++)
        {
            for (int X = 0; X < baseTilesCoveredWidth; X++)
            {
                int tileY = tileToMap->coordinate.Y * baseTilesCoveredHeight + Y;
                int tileX = tileToMap->coordinate.X * baseTilesCoveredWidth + X;
                byte* value = &tileToMap->managedResource->residency[tileToMap->face][tileY * tileToMap->managedResource->subresourceTilings[0].WidthInTiles + tileX];
                *value = min(tileToMap->mipLevel * 16, *value);
            }
        }

        m_mappedTileList.push_back(tileToMap);
    }

    // Use a single call to update all tile mappings.
    for (auto perResourceArguments : coalescedMapArguments)
    {
#ifdef _DEBUG
        if (m_debugMode)
        {
            for (size_t i = 0; i < perResourceArguments.second.coordinates.size(); i++)
            {
                std::ostringstream tileMappingMessage;
                tileMappingMessage << "ResidencyManager::ProcessQueues : Updating mapping for tile ";
                tileMappingMessage << "[" << perResourceArguments.second.coordinates[i].Subresource << "][" << perResourceArguments.second.coordinates[i].Y << "][" << perResourceArguments.second.coordinates[i].X << "]";
                if (perResourceArguments.second.rangeFlags[i] == D3D11_TILE_RANGE_NULL)
                {
                    tileMappingMessage << " --> NULL" << std::endl;
                }
                else
                {
                    tileMappingMessage << " --> Tile #" << perResourceArguments.second.physicalOffsets[i] << std::endl;
                }
                OutputDebugStringA(tileMappingMessage.str().c_str());
            }
        }
#endif

        std::vector<UINT> rangeCounts(perResourceArguments.second.rangeFlags.size(), 1);
        D3D11_TILE_REGION_SIZE size = {};

        size.NumTiles = 1;
        std::vector<D3D11_TILE_REGION_SIZE> sizes(perResourceArguments.second.rangeFlags.size(),size);
        DX::ThrowIfFailed(
            context->UpdateTileMappings(
                perResourceArguments.first,
                (UINT)perResourceArguments.second.coordinates.size(),
                perResourceArguments.second.coordinates.data(),
                sizes.data(),
                m_tilePool.Get(),
                (UINT)perResourceArguments.second.rangeFlags.size(),
                perResourceArguments.second.rangeFlags.data(),
                perResourceArguments.second.physicalOffsets.data(),
                rangeCounts.data(),
                0
                )
            );

        context->TiledResourceBarrier(NULL, perResourceArguments.first);
    }

    // Finally, copy the contents of the tiles mapped this frame.
    for (auto perResourceArguments : coalescedMapArguments)
    {
        for (size_t i = 0; i < perResourceArguments.second.coordinates.size(); i++)
        {
            //if (perResourceArguments.second.rangeFlags[i] != D3D11_TILE_RANGE_NULL || perResourceArguments.second.physicalOffsets[i] != m_defaultTileIndex )
            if (perResourceArguments.second.physicalOffsets[i] != m_defaultTileIndex)
            {
                D3D11_TILE_REGION_SIZE regionSize = {} ;
                
                regionSize.NumTiles = 1;
                context->UpdateTiles(
                    perResourceArguments.first,
                    &perResourceArguments.second.coordinates[i],
                    &regionSize,
                    perResourceArguments.second.tilesToMap.front()->tileData.data(),
                    0
                    );
                perResourceArguments.second.tilesToMap.front()->tileData.clear();
                perResourceArguments.second.tilesToMap.pop_front();
            }
            context->TiledResourceBarrier(NULL, perResourceArguments.first);
        }
    }

    // Update residency textures with the new residency data.
    for (auto resource : m_managedResources)
    {
        int baseWidthInTiles = resource->subresourceTilings[0].WidthInTiles;
        int baseHeightInTiles = resource->subresourceTilings[0].HeightInTiles;
        int baseMaxTileDimension = max(baseWidthInTiles, baseHeightInTiles);
        std::vector<byte> residencyData(baseMaxTileDimension * baseMaxTileDimension);
        for (int face = 0; face < 6; face++)
        {
            for (int Y = 0; Y < baseMaxTileDimension; Y++)
            {
                int tileY = (Y * baseHeightInTiles) / baseMaxTileDimension;
                for (int X = 0; X < baseMaxTileDimension; X++)
                {
                    int tileX = (X * baseWidthInTiles) / baseMaxTileDimension;
                    residencyData[Y * baseMaxTileDimension + X] = resource->residency[face][tileY * baseWidthInTiles + tileX];
                }
            }
            context->UpdateSubresource(
                resource->residencyTexture.Get(),
                face,
                NULL,
                residencyData.data(),
                baseMaxTileDimension,
                0
                );
        }
    }
    */
}

void ResidencyManager::SetDebugMode(bool value)
{
    //m_debugMode = value;
}

void ResidencyManager::Reset()
{
    /*
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Clear tracked tiles and residency map.
    m_trackedTiles.clear();
    m_seenTileList.clear();
    m_loadingTileList.clear();
    m_mappedTileList.clear();

    for (auto resource : m_managedResources)
    {
        for (int face = 0; face < 6; face++)
        {
            resource->residency[face].clear();
            resource->residency[face].resize(resource->subresourceTilings[0].WidthInTiles * resource->subresourceTilings[0].HeightInTiles, 0xFF);
        }
    }

    // Reset tile mappings to NULL.
    for (auto resource : m_managedResources)
    {
        UINT flags = D3D11_TILE_RANGE_NULL;
        DX::ThrowIfFailed(
            context->UpdateTileMappings(
                resource->texture,
                1,
                nullptr, // Use default coordinate of all zeros.
                nullptr, // Use default region of entire resource.
                nullptr,
                1,
                &flags,
                nullptr,
                nullptr,
                0
                )
            );
    }

    if (m_deviceResources->GetTiledResourcesTier() <= D3D11_TILED_RESOURCES_TIER_1)
    {
        // On Tier-1 devices, applications must ensure that NULL-mapped tiles are never accessed.
        // Because the mapping heuristic in this sample is only approximate, it is safest to map
        // all tiles to a dummy tile that will serve as the NULL tile.

        // Create a temporary texture to clear the dummy tile to zero.
        D3D11_TEXTURE2D_DESC tempTextureDesc = {};
        tempTextureDesc.Width = 256;
        tempTextureDesc.Height = 256;
        tempTextureDesc.MipLevels = 1;
        tempTextureDesc.ArraySize = 1;
        tempTextureDesc.Format = DXGI_FORMAT_R8_UNORM;
        tempTextureDesc.SampleDesc.Count = 1;
        tempTextureDesc.Usage = D3D11_USAGE_DEFAULT;
        tempTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TILED;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> tempTexture;
        DX::ThrowIfFailed(device->CreateTexture2D(&tempTextureDesc, nullptr, &tempTexture));

        // Map the single tile in the buffer to physical tile 0.
        D3D11_TILED_RESOURCE_COORDINATE startCoordinate = {};
        D3D11_TILE_REGION_SIZE regionSize = {};
        regionSize.NumTiles = 1;
        UINT rangeFlags = D3D11_TILE_RANGE_REUSE_SINGLE_TILE;
        m_defaultTileIndex = m_reservedTiles++;
        DX::ThrowIfFailed(
            context->UpdateTileMappings(
                tempTexture.Get(),
                1,
                &startCoordinate,
                &regionSize,
                m_tilePool.Get(),
                1,
                &rangeFlags,
                &m_defaultTileIndex,
                nullptr,
                0
                )
            );

        // Clear the tile to zeros.
        byte defaultTileData[SampleSettings::TileSizeInBytes];
        ZeroMemory(defaultTileData, sizeof(defaultTileData));
        context->UpdateTiles(tempTexture.Get(), &startCoordinate, &regionSize, defaultTileData, 0);

        // Map all tiles to the dummy physical tile.
        for (auto resource : m_managedResources)
        {
            regionSize.NumTiles = resource->totalTiles;
            DX::ThrowIfFailed(
                context->UpdateTileMappings(
                    resource->texture,
                    1,
                    &startCoordinate,
                    &regionSize,
                    m_tilePool.Get(),
                    1,
                    &rangeFlags,
                    &m_defaultTileIndex,
                    nullptr,
                    0
                    )
                );
            context->TiledResourceBarrier(NULL, resource->texture);
        }
    }
    */
}

std::wstring g_diffuseFilename;

ID3D11ShaderResourceView* ResidencyManager::ManageTexture(ID3D11Texture2D* texture, const std::wstring& filename)
{
    /*
    auto device = m_deviceResources->GetD3DDevice();


    auto resource = std::shared_ptr<ManagedTiledResource>(new ManagedTiledResource);
    resource->texture = texture;
    texture->GetDesc(&resource->textureDesc);
    UINT subresourceTilings = resource->textureDesc.MipLevels * resource->textureDesc.ArraySize;
    resource->subresourceTilings.resize(subresourceTilings);
    device->GetResourceTiling(
        texture,
        &resource->totalTiles,
        &resource->packedMipDesc,
        &resource->tileShape,
        &subresourceTilings,
        0,
        resource->subresourceTilings.data()
        );
    DX::ThrowIfFailed(subresourceTilings == resource->textureDesc.MipLevels * resource->textureDesc.ArraySize ? S_OK : E_UNEXPECTED);

    if (resource->textureDesc.Format == DXGI_FORMAT_BC1_UNORM)
        g_diffuseFilename = filename;

    resource->loader.reset(new TileLoader(filename, &resource->subresourceTilings, false));

    // Create the residency texture.
    D3D11_TEXTURE2D_DESC textureDesc = {};
    
    textureDesc.Width = max(resource->subresourceTilings[0].WidthInTiles, resource->subresourceTilings[0].HeightInTiles);
    textureDesc.Height = textureDesc.Width;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 6;
    textureDesc.Format = DXGI_FORMAT_R8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    DX::ThrowIfFailed(device->CreateTexture2D(&textureDesc, nullptr, &resource->residencyTexture));

    // Create the shader resource view that will be used by both the terrain renderer and the visualizer.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    shaderResourceViewDesc.TextureCube.MipLevels = 1;
    DX::ThrowIfFailed(device->CreateShaderResourceView(resource->residencyTexture.Get(), &shaderResourceViewDesc, &resource->residencyTextureView));

    // Allocate space for the saved residency data.
    for (int face = 0; face < 6; face++)
    {
        resource->residency[face].resize(resource->subresourceTilings[0].WidthInTiles * resource->subresourceTilings[0].HeightInTiles, 0xFF);
    }

    m_managedResources.push_back(resource);

    // Return the residency view.
    return resource->residencyTextureView.Get();
    */
}

task<void> ResidencyManager::InitializeManagedResourcesAsync()
{
    /*
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    Reset();

    // Load all packed MIPs.
    std::vector<task<void>> tileLoadTasks;
    for (auto resource : m_managedResources)
    {
        if (resource->packedMipDesc.NumPackedMips == 0)
        {
            continue;
        }

        // Start the load tasks.
        D3D11_TILED_RESOURCE_COORDINATE coordinate = {};
        for (short face = 0; face < 6; face++)
        {
            for (int i = 0; i < resource->packedMipDesc.NumPackedMips; i++)
            {
                short mip = resource->textureDesc.MipLevels - i - 1;
                coordinate.Subresource = face * resource->textureDesc.MipLevels + mip;
                tileLoadTasks.push_back(resource->loader->LoadTileAsync(coordinate).then([this, resource, coordinate](std::vector<byte> tileData)
                {
                    auto context = m_deviceResources->GetD3DDeviceContext();
                    // Packed tiles may not be updated using the UpdateTiles API, so we use the UpdateSubresource API instead.
                    UINT pitch = resource->tileShape.WidthInTexels;
                    switch(resource->textureDesc.Format)
                    {
                    case DXGI_FORMAT_BC1_UNORM: // 8 bytes per 4-wide block.
                        pitch *= 8 / 4;
                        break;
                    case DXGI_FORMAT_BC5_SNORM: // 16 bytes per 4-wide block.
                        pitch *= 16 / 4;
                        break;
                    case DXGI_FORMAT_B8G8R8A8_UNORM: // 4 bytes per pixel.
                        pitch *= 4;
                        break;
                    case DXGI_FORMAT_R16G16_SNORM: // 4 bytes per pixel.
                        pitch *= 4;
                        break;
                    default:
                        DX::ThrowIfFailed(E_NOTIMPL); // This code only supports the four formats used by the assets included in this sample.
                        break;
                    }
                    context->UpdateSubresource(resource->texture, coordinate.Subresource, nullptr, tileData.data(), pitch, 0);
                }));
            }
        }
    }

    return when_all(tileLoadTasks.begin(), tileLoadTasks.end());
    */
}

void ResidencyManager::SetBorderMode(bool value)
{
    /*
    m_managedResources[0]->loader.reset(new TileLoader(g_diffuseFilename, &m_managedResources[0]->subresourceTilings, value));
    */
}
