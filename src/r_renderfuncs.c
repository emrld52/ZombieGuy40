#include <stdio.h>
#include <stdlib.h>

// sokol stuff

#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include "../deps/sokol_glue.h"
#include "../deps/sokol_time.h"
#include "../deps/sokol_log.h"

// use stb image for image decoding

#include "../deps/stb_image.h"

// shaders

#include "../shaders/shaders.glsl.h"

// cglm gl math

#include "../deps/cglm/cglm.h"

// memcpy stuff for some reason, no idea why this fixes things. didnt need it when i was doing all this in main but need it here

#include <string.h>

// include parent header

#include "r_renderfuncs.h"

// global state

#include "g_state.h"

#define MAX_DRAW_CALLS 128

// rendering state

static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
    quad_vs_params_t vertex_shader_params;
} state;

// test image for now
sg_image bob;

uint64_t raw_delta_time = 0;

void init_rendering()
{
    sg_setup(&(sg_desc)
    {
        .environment = sglue_environment(),
        .logger.func = slog_func
    });

    // set up frame time count

    stm_setup();

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

    // debug stuff

    printf("width %d, height %d, channels %d", width, height, channels);
    
    // make image out of data just loaded in ram, send to vram

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

    // free from ram

    stbi_image_free(pixels);

    // create shader out of basic shaders wrote before
    sg_shader shd = sg_make_shader(quad_shader_desc(sg_query_backend()));

    // shader pipeline

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

// init draw queue that we will append draw calls to as sprites. circular queue. max draw calls of 128

sprite draw_queue[MAX_DRAW_CALLS];

//start of queue, end of queue, length of queue

int head = 0, tail = 0, len = 0;

void draw_call(sprite s)
{
    if(len != MAX_DRAW_CALLS) {
        draw_queue[tail] = s;

        tail = (tail + 1) % MAX_DRAW_CALLS;
        len += 1;
    }
}

void end_call()
{
    if(len > 0)
    {
        memset(&draw_queue[head], 0, sizeof(sprite));

        head = (head + 1) % MAX_DRAW_CALLS;
        len -= 1;
    }
}

void draw_game()
{
    // set global delta-time

    //delta_time = (float)(stm_ms(stm_laptime(&raw_delta_time)) / 1000);

    // print frame-time

    printf("frametime : %f \n", stm_ms(stm_laptime(&raw_delta_time)) / 1000);

    sg_begin_pass(&(sg_pass) { .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    // if no draw calls just render nothing

    if(len > 0) {
        // go through all draw calls

        for (int i = 0; i <= len; i++)
        {
            // define simple ortho matrix to avoid stretching, keep aspect ratios and such consistent

            mat4 proj;
            glm_ortho(0.0f, sapp_width(), sapp_height(), 0.0f, -1.0f, 1.0f, proj);
            memcpy(state.vertex_shader_params.projection, proj, sizeof(float) * 16);

            memcpy(state.vertex_shader_params.position, draw_queue[head].pos, sizeof(vec3) * 1);

            sg_apply_uniforms(UB_quad_vs_params, &SG_RANGE(state.vertex_shader_params));

            sg_draw(0, 6, 1);

            // confirm draw call complete

            end_call();
        }
    }
    
    sg_end_pass();

    sg_commit();
}