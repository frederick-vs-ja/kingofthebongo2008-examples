#include "precompiled.h"
#include <cstdint>

#include <gx/gx_default_application.h>
#include <gx/gx_view_port.h>

#include <sys/sys_profile_timer.h>

#include "d3dx12.h"

class sample_application : public gx::default_application
{
    typedef gx::default_application base;

public:

    sample_application(const wchar_t* window_title) : base(window_title)
        , m_elapsed_update_time(0.0)
        , m_rtv_heap( m_context.m_device.get(), 3 )
        , m_rtv_cpu_heap( m_rtv_heap.create_cpu_heap() )
    {
        m_wait_back_buffer = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

        auto device = this->m_context.m_device.get();

        m_wait_back_buffer_fence = d3d12x::create_fence( device );
        m_frame_index = 1;

        for (auto i = 0U; i < m_swap_buffer_count; ++i)
        {
            m_command_allocators[i] = d3d12x::create_command_allocator(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
            m_command_lists[i] = d3d12x::create_graphics_command_list(device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocators[i], nullptr);

            d3d12::throw_if_failed(m_command_lists[i]->Close());
        }


        m_command_queue = m_context.m_direct_command_queue;
        m_index_last_swap_buffer = 0;
        m_render_target = dxgi::get_buffer(m_context.m_swap_chain, m_index_last_swap_buffer);
        device->CreateRenderTargetView(m_render_target.get(), nullptr, m_rtv_cpu_heap(0) );
    }

    ~sample_application()
    {
        wait_gpu_for_all_back_buffers();
        CloseHandle( m_wait_back_buffer );
    }

    void shutdown()
    {
        wait_gpu_for_all_back_buffers();
    }

protected:

    virtual void on_render_scene()
    {
        using namespace d3d12x;
        using namespace d3d12;

        auto frame_index = m_frame_index % m_swap_buffer_count;

        throw_if_failed(m_command_allocators[frame_index]->Reset() );
        throw_if_failed(m_command_lists[frame_index]->Reset(m_command_allocators[frame_index], nullptr ));
        
        apply_resource_barrier(m_command_lists[frame_index], resource_barrier::present_rt(m_render_target));

        m_command_lists[frame_index]->OMSetRenderTargets(1, &m_rtv_cpu_heap(0), true, nullptr);

        float clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };

        m_command_lists[frame_index]->ClearRenderTargetView(m_rtv_cpu_heap(0), clear_color, 0, nullptr);

        apply_resource_barrier(m_command_lists[frame_index], resource_barrier::rt_present(m_render_target));

        throw_if_failed(m_command_lists[frame_index]->Close());

        // Execute the command list.
        ID3D12CommandList* command_lists[] = { m_command_lists[frame_index].get() };
        m_command_queue->ExecuteCommandLists( _countof(command_lists), command_lists);
    }

    void render_scene()
    {
        on_render_scene();
    }

    void wait_gpu_for_back_buffer()
    {
        const auto frame_index = m_frame_index;

        //signal for stop rendering this frame
        d3d12::throw_if_failed( m_command_queue->Signal(m_wait_back_buffer_fence, frame_index) );

        m_frame_index++;

        //wait for the gpu to finish frame
        auto completed_frame = m_wait_back_buffer_fence->GetCompletedValue();
        if ( completed_frame < frame_index - 1  )
        {
            m_wait_back_buffer_fence->SetEventOnCompletion( frame_index - 1 , this->m_wait_back_buffer );
            WaitForSingleObject(m_wait_back_buffer, INFINITE);
        }
    }

    void wait_gpu_for_all_back_buffers()
    {
        
        //signal for stop rendering
        d3d12::throw_if_failed(m_command_queue->Signal(m_wait_back_buffer_fence, ++m_frame_index));
        m_wait_back_buffer_fence->SetEventOnCompletion(m_frame_index, this->m_wait_back_buffer);
        WaitForSingleObject(m_wait_back_buffer, INFINITE);
    }

    virtual void on_update_scene()
    {

    }

    void update_scene()
    {
        on_update_scene();
    }

    void on_update() override
    {
        sys::profile_timer timer;

        update_scene();

        //Measure the update time and pass it to the render function
        m_elapsed_update_time = timer.milliseconds();
    }

    void on_render_frame() override
    {
        sys::profile_timer timer;
        on_render_scene();
    }

    void on_post_render_frame() override
    {
        wait_gpu_for_back_buffer();

        m_index_last_swap_buffer++;
        m_index_last_swap_buffer = m_index_last_swap_buffer % 3;
        m_render_target = dxgi::get_buffer(m_context.m_swap_chain, m_index_last_swap_buffer);
        m_context.m_device->CreateRenderTargetView(m_render_target.get(), nullptr, m_rtv_cpu_heap(0));
    }

    void on_resize(uint32_t width, uint32_t height) override
    {
        wait_gpu_for_back_buffer();

        m_render_target.reset();

        base::on_resize(width, height);

        //reacuire render targets
        m_index_last_swap_buffer = 0;
        m_render_target = dxgi::get_buffer(m_context.m_swap_chain, m_index_last_swap_buffer);
        m_context.m_device->CreateRenderTargetView(m_render_target.get(), nullptr, m_rtv_cpu_heap(0));

        //Reset view port dimensions
        m_view_port.set_dimensions(width, height);
    }

protected:

    gx::view_port                           m_view_port;
    double                                  m_elapsed_update_time;

    HANDLE                                  m_wait_back_buffer;
    d3d12::fence                            m_wait_back_buffer_fence;
    uint32_t                                m_frame_index;

    static const uint32_t                   m_swap_buffer_count = 3;
    d3d12::command_allocator                m_command_allocators[m_swap_buffer_count];
    d3d12::graphics_command_list            m_command_lists[m_swap_buffer_count];
    d3d12::command_queue                    m_command_queue;

    d3d12::resource                         m_render_target;

    d3d12x::descriptor_heap< D3D12_DESCRIPTOR_HEAP_TYPE_RTV> m_rtv_heap;
    d3d12x::cpu_descriptor_heap                              m_rtv_cpu_heap;

    uint32_t                                m_index_last_swap_buffer = 0;
    
};

int32_t wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int       nCmdShow )
{
    // Enable the D3D12 debug layer.
    {
        ID3D12Debug* debugController;
        D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        if (debugController)
        {
            debugController->EnableDebugLayer();
        }
    }

    auto app = new sample_application(L"qcrm");

    app->run();
    app->shutdown();
    
    delete app;

    return 0;
}




