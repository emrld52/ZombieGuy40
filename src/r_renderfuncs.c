// include parent header

#include "r_renderfuncs.h"

// cglm

#include "../deps/cglm/cglm.h"

// sokol stuff

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
    quad_vs_params_t vertex_shader_params;
} state;

// test image for now
sg_image bob;

void init_rendering()
{
    sg_setup(&(sg_desc)
    {
        .environment = sglue_environment()
    });

    // define quad, along with indicies buffer
    
    const float vertices[] =
    {
        // pos              // uv
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // top left
        32.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
        0.0f, 32.0f, 0.0f, 0.0f, 1.0f, // bottom left
        32.0f, 32.0f, 0.0f,  1.0f, 1.0f // bottom right
    };

    const uint16_t indices[] = {
        0, 1, 3,
        0, 2, 3
    };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc)
    {
        .data = SG_RANGE(vertices),
        .label = "vertex-buffer"
    });

    state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .usage.index_buffer = true,
        .data = SG_RANGE(indices),
        .label = "index-buffer"
    });

    // load test image

    int width=0, height=0, channels=0;

    unsigned char* pixels = stbi_load("assets/img/bob.png", &width, &height, &channels, 0);

    printf("width %d, height %d, channels %d", width, height, channels);
    
    bob = sg_make_image(&(sg_image_desc){
        .width = width,
        .height = height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data.mip_levels[0] = (sg_range)
        {
            .ptr = pixels,
            .size = (size_t)(width * height * channels)
        }
    });

    state.bind.views[0] = sg_make_view(&(sg_view_desc){
        .texture = { .image = bob },
        .label = "tex-view",
    });

    // create a sampler object with default attributes
    state.bind.samplers[1] = sg_make_sampler(&(sg_sampler_desc){
        .label = "tex-sampler"
    });

    stbi_image_free(pixels);

    // create shader out of basic shaders wrote before
    sg_shader shd = sg_make_shader(quad_shader_desc(sg_query_backend()));

    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,

        //define how to read input data into shader
        .layout = {
            .buffers[0].stride = 5 * sizeof(float),
            .attrs = {
                [ATTR_quad_a_pos].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_quad_a_uv].format = SG_VERTEXFORMAT_FLOAT2
            }
        },

        .index_type = SG_INDEXTYPE_UINT16,

        .label = "quad-pipeline"
    });

    state.pass_action = (sg_pass_action)
    {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}}
    };
}

void draw_game()
{
    sg_begin_pass(&(sg_pass) { .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    // define simple ortho matrix to avoid stretching, keep aspect ratios and such consistent

    mat4 proj;
    glm_ortho(0.0f, sapp_width(), sapp_height(), 0.0f, -1.0f, 1.0f, proj);
    memcpy(state.vertex_shader_params.projection, proj, sizeof(float) * 16);

    memcpy(state.vertex_shader_params.position, (vec3){ 0, 0, 0 }, sizeof(vec3) * 1);

    sg_apply_uniforms(UB_quad_vs_params, &SG_RANGE(state.vertex_shader_params));

    sg_draw(0, 6, 1);
    
    sg_end_pass();

    sg_commit();
}