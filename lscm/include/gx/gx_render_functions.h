#ifndef __GX_RENDER_FUNCTIONS_H__
#define __GX_RENDER_FUNCTIONS_H__

#include <cstdint>
#include <limits>

#include <math/math_half.h>
#include <math/math_matrix.h>

#include <d3d11/d3d11_helpers.h>

#include <gx/shaders/gx_shader_full_screen.h>

namespace gx
{
    class full_screen_draw
    {
        public:

        full_screen_draw ( ID3D11Device* device ) : 
            m_shader(device)
            , m_input_layout (device, m_shader)
        {
            using namespace math;

            struct vertex
            {
                half v[4];
                half uv[2];
            };

            struct vertex_float
            {
                float v[4];
                float uv[2];
            };

            const vertex_float v_1[ 6 + 2 ] =
            { 
                 { {-1.0f,	-1.0f,	0.0f, 1.0f},  {0.0f, 1.0f}},
                 { {-1.0f,	 1.0f,	0.0f, 1.0f},  {0.0f, 0.0f}},
                 { {1.0f,	 1.0f,	0.0f, 1.0f},  {1.0f, 0.0f}},

                 { {1.0f,	 1.0f,	0.0f, 1.0f} , {1.0f, 0.0f}},
                 { {1.0f,	-1.0f,	0.0f, 1.0f} , {1.0f, 1.0f}},
                 { {-1.0f,	-1.0f,	0.0f, 1.0f} , {0.0f, 1.0f}},

                 { {0.0f,	0.0f,	0.0f, 0.0f} , {0.0f,0.0f}}, //padding
                 { {0.0f,	0.0f,	0.0f, 0.0f} , {0.0f,0.0f}}, //padding
            };

            __declspec( align(16) ) math::half h_1 [ 40 ];

            math::convert_f32_f16_stream(reinterpret_cast<const float*> (&v_1[0]), static_cast<uint32_t>(40), &h_1[0] );
            m_geometry = d3d11::create_immutable_vertex_buffer( device, &h_1[0],  6 * sizeof(vertex) );
        }
            

        void    draw ( ID3D11DeviceContext* device_context )
        {

            d3d11::ia_set_input_layout( device_context, m_input_layout );
            d3d11::vs_set_shader( device_context, m_shader );
            d3d11::ia_set_primitive_topology(device_context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            d3d11::ia_set_vertex_buffer ( device_context, m_geometry, 12 );

            device_context->Draw( 6, 0 );
        }

        shader_full_screen          m_shader;

        shader_full_screen_layout   m_input_layout;
        d3d11::ibuffer_ptr          m_geometry;
    };

}



#endif

