#include <stdio.h>
#include <stdlib.h>

#define SOKOL_IMPL
#define SOKOL_D3D11

#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include "../deps/sokol_glue.h"
#include "../shaders/triangle_shader.h"

// rendering state, only visible for this file, doesnt overwrite game state
static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void)
{
    sg_setup(&(sg_desc)
    {
        .environment = sglue_environment()
    });

    // define triangle
    float verticies[] =
    {
        // pos              // col
        0.0f, 0.5f, 0.5f,   1, 0, 0, 1,
        0.5f, -0.5f, 0.5f,  0, 1, 0, 1,
        -0.5f, -0.5f, 0.5f, 0, 0, 1, 1
    };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc)
    {
        .data = SG_RANGE(verticies),
        .label = "vertex-buffer"
    });

    // create shader out of basic shaders wrote before
    sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,

        //define how to read input data into shader
        .layout = {
            .attrs = {
                [ATTR_triangle_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_triangle_color0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },

        .label = "triangle-pipeline"
    });

    state.pass_action = (sg_pass_action)
    {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}}
    };
}


void frame()
{
    sg_begin_pass(&(sg_pass) { .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    sg_draw(0, 3, 1);
    
    sg_end_pass();

    sg_commit();
}

void cleanup()
{
    sg_shutdown();
}

void event()
{

}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc) {
        .width = 640,
        .height = 480,
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "fart"
    };
}