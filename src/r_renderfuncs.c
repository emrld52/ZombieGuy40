#include <stdio.h>
#include <stdlib.h>

// sokol stuff

#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include "../deps/sokol_glue.h"
#include "../deps/sokol_time.h"
#include "../deps/sokol_log.h"

// libs

#include "../deps/cglm/cglm.h"
#include <string.h>
#include "../deps/stb_image.h"

// shaders

#include "../shaders/shaders.glsl.h"

// source

#include "r_renderfuncs.h"
#include "g_state.h"

// rendering state

static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
    quad_vs_params_t vertex_shader_params;
    quad_fs_params_t fragment_shader_params;
} state;

// texture atlas
sg_image texture_atlas;

uint64_t global_raw_delta_time = 0;
float global_delta_time = 0;

int render_game_width = 640;

// define simple ortho matrix to avoid stretching, keep aspect ratios and such consistent

mat4 proj;

sprite make_sprite(vec2 pos, vec2 sprite_coord, bool ui, vec2 resolution)
{
    return (sprite)
    {
        .pos[0] = pos[0], .pos[1] = pos[1],
        .sprite_coord[0] = sprite_coord[0], .sprite_coord[1] = sprite_coord[1],
        .ui = ui,
        .resolution[0] = resolution[0], .resolution[1] = resolution[1]
    };
}

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
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, // top left
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // bottom left
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f // bottom right
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

    // load texture atlas, assets0.zmb is just a renamed png file to slightly obfuscate the game. also zmb because zombie

    int width=0, height=0, channels=0;

    // check root folder or bin for assets

    unsigned char* pixels = stbi_load("assets0.zmb", &width, &height, &channels, 0);
    if(pixels == NULL) pixels = stbi_load("bin/assets0.zmb", &width, &height, &channels, 0);
    
    // make image out of data just loaded in ram, send to vram

    texture_atlas = sg_make_image(&(sg_image_desc){
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
        .texture = { .image = texture_atlas },
        .label = "tex-view",
    });

    // create a sampler object with default attributes
    state.bind.samplers[1] = sg_make_sampler(&(sg_sampler_desc){
        .label = "tex-sampler"
    });

    // set texture atlas dimensions in uniform

    state.fragment_shader_params.texture_atlas_dimensions[0] = 1280;
    state.fragment_shader_params.texture_atlas_dimensions[1] = 1280;

    // how many rows and columns

    state.fragment_shader_params.sprite_count[0] = (float)TEXTURE_ATLAS_SPRITE_X_COUNT;
    state.fragment_shader_params.sprite_count[1] = (float)TEXTURE_ATLAS_SPRITE_Y_COUNT;

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

        .label = "quad-pipeline",

        .colors[0].blend.enabled = true,
        .colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
        .colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .colors[0].blend.op_rgb = SG_BLENDOP_ADD,

        .colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE,
        .colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .colors[0].blend.op_alpha = SG_BLENDOP_ADD,

        .colors[0].write_mask = SG_COLORMASK_RGBA
    });

    // define simple sky blue for pass action

    state.pass_action = (sg_pass_action)
    {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}}
    };
}

void begin_drawing()
{
    // set global delta-time, get time in ticks, convert to miliseconds, divide by 1000 to get the time that is out of one second to multiply movement by

    global_delta_time = stm_ms(stm_laptime(&global_raw_delta_time)) / 1000;

    //glm_vec2_lerp(global_camera_position, (vec2){16 + loaded_scene->entities[0].position[0] - sapp_width() / 2, 16 + loaded_scene->entities[0].position[1] - sapp_height() / 2}, 8 * global_delta_time * loaded_scene->scene_game_speed, global_camera_position);
    glm_vec2_lerp(loaded_scene->scene_camera_position, GLM_VEC2_ZERO, 8 * global_delta_time * loaded_scene->scene_game_speed, loaded_scene->scene_camera_position);

    sg_begin_pass(&(sg_pass) { 
        .action = state.pass_action, 
        .swapchain = sglue_swapchain() 
    });

    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    // update camera position in shader

    state.vertex_shader_params.cam_position[0] = loaded_scene->scene_camera_position[0];
    state.vertex_shader_params.cam_position[1] = loaded_scene->scene_camera_position[1];

    // each frame rebuild matrix in the event that window has been stretched, might remove when i implement fixed aspect ratio
    glm_ortho(0.0f, 640, 480, 0.0f, -1.0f, 1.0f, proj);
    memcpy(state.vertex_shader_params.projection, proj, sizeof(float) * 16);
}

void draw_call(sprite s)
{
    memcpy(state.vertex_shader_params.position, s.pos, sizeof(vec3) * 1);

    state.fragment_shader_params.sprite_coord[0] = s.sprite_coord[0];
    state.fragment_shader_params.sprite_coord[1] = s.sprite_coord[1];

    state.fragment_shader_params.texture_offset[0] = s.sprite_offset[0];
    state.fragment_shader_params.texture_offset[1] = s.sprite_offset[1];

    // pass in sprite resolution

    state.vertex_shader_params.scale[0] = s.resolution[0];
    state.vertex_shader_params.scale[1] = s.resolution[1];

    if(s.flip_x) 
    {
        state.vertex_shader_params.flip_x = 1;
        state.vertex_shader_params.position[0] += s.resolution[0];
    }
    else state.vertex_shader_params.flip_x = 0;

    if(s.ui == true)
    {
        state.vertex_shader_params.cam_position[0] = 0;
        state.vertex_shader_params.cam_position[1] = 0;

        sg_apply_uniforms(UB_quad_vs_params, &SG_RANGE(state.vertex_shader_params));

        state.vertex_shader_params.cam_position[0] = loaded_scene->scene_camera_position[0];
        state.vertex_shader_params.cam_position[1] = loaded_scene->scene_camera_position[1];
    }
    else sg_apply_uniforms(UB_quad_vs_params, &SG_RANGE(state.vertex_shader_params));

    sg_apply_uniforms(UB_quad_fs_params, &SG_RANGE(state.fragment_shader_params));

    sg_draw(0, 6, 1);
}

void end_draw()
{
    sg_end_pass();

    sg_commit();
}

void camera_shake(float magnitude)
{
    float angle = ((float)rand() / (float)RAND_MAX) * 2.0f * M_PI;
    vec2 dir; 
    dir[0] = cosf(angle) * magnitude;
    dir[1] = sinf(angle) * magnitude;

    glm_vec2_add(loaded_scene->scene_camera_position, dir, loaded_scene->scene_camera_position);
}