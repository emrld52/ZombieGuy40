#include <stdio.h>
#include <stdlib.h>

// sokol stuff

#define SOKOL_IMPL
#define SOKOL_D3D11

#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include "../deps/sokol_glue.h"

// use stb image for image decoding

#define STB_IMAGE_IMPLEMENTATION

#include "../deps/stb_image.h"

// shaders

#include "../shaders/shaders.glsl.h"

// rendering state
static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

// test image for now
sg_image bob;

static void init(void)
{
    sg_setup(&(sg_desc)
    {
        .environment = sglue_environment()
    });

    // define quad, along with indicies buffer
    float verticies[] =
    {
        // pos              // col
        0.5f, 0.5f, 0.0f,   1, 0, 0, 1, // top right
        0.5f, -0.5f, 0.0f,  0, 1, 0, 1, // bottom right
        -0.5f, -0.5f, 0.0f, 0, 0, 1, 1, // bottom left
        -0.5f, 0.5f, 0.0f,  0, 1, 0, 1, // top left
    };

    uint16_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc)
    {
        .data = SG_RANGE(verticies),
        .label = "vertex-buffer"
    });

    state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .usage.index_buffer = true,
        .data = SG_RANGE(indices),
        .label = "index-buffer"
    });

    // create shader out of basic shaders wrote before
    sg_shader shd = sg_make_shader(quad_shader_desc(sg_query_backend()));

    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,

        //define how to read input data into shader
        .layout = {
            .attrs = {
                [ATTR_quad_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_quad_color0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },

        .index_type = SG_INDEXTYPE_UINT16,

        .label = "quad-pipeline"
    });

    state.pass_action = (sg_pass_action)
    {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}}
    };

    // load test image, currently not working

    /*
    int width, height, channels;

    unsigned char* pixels = stbi_load("assets/img/bob.png", &width, &height, &channels, 4);

    bob = sg_make_image(&(sg_image_desc){
        .width = width,
        .height = height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data = SG_RANGE(pixels),
        .label = "bob"
    });

    stbi_image_free(pixels);*/
}


void frame()
{
    sg_begin_pass(&(sg_pass) { .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    sg_draw(0, 6, 1);
    
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
        .window_title = "ZombieGuy40"
    };
}