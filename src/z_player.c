// source

#include "z_player.h"
#include "r_renderfuncs.h"
#include "g_state.h"
#include "z_coreloop.h"
#include "s_entities.h"
#include "s_scene.h"

// libraries

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include <stdbool.h>

// player-movmement variables

#define PLAYER_MAX_SPEED 256.0f * 0.75f
#define PLAYER_GRAVITY 1024.0f
#define PLAYER_JUMP_FORCE 512.0f

entity *ply;

// init player position and velocity

void player_init()
{
    ply = make_entity_in_scene(loaded_scene);

    glm_vec3_copy((vec2){ (sapp_width() / 2) - 32, 0.0f }, ply->position);
    glm_vec3_copy((vec2){ 0.0f, 0.0f }, ply->velocity);
    glm_vec2_copy((vec2){ 32, 32 }, ply->sprite_data.resolution);
    glm_vec2_copy((vec2){ 1, 1 }, ply->sprite_data.sprite_coord);
    glm_vec2_copy((vec2){16, ply->sprite_data.resolution[1]}, ply->hit_box);
    glm_vec2_copy((vec2){8, 0}, ply->hit_box_offset);
    ply->gravity = PLAYER_GRAVITY;
}

void player_loop()
{
    if(ply->is_grounded && global_input.keysPressed[SAPP_KEYCODE_SPACE]) entity_override_velocity(ply, (vec2){ply->velocity[0], -PLAYER_JUMP_FORCE});

    if(global_input.keysPressed[SAPP_KEYCODE_D]) ply->velocity[0] = PLAYER_MAX_SPEED;
    else if(global_input.keysPressed[SAPP_KEYCODE_A]) ply->velocity[0] = -PLAYER_MAX_SPEED;
    else ply->velocity[0] = 0;

    if(global_input.keysPressed[SAPP_KEYCODE_E]) camera_shake(5.0f);
}

void player_reset()
{

}