// source

#include "z_player.h"
#include "r_renderfuncs.h"
#include "g_state.h"
#include "z_coreloop.h"
#include "s_entities.h"
#include "s_scene.h"
#include "s_animation.h"

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

    animator_init(&ply->animator_component);
    play_animation(&ply->animator_component, &ANIM_PLAYER_IDLE);
}

void player_loop()
{
    // animation states and jump logic

    if(ply->is_grounded) 
    {
        if(global_input.keysPressed[SAPP_KEYCODE_SPACE]) entity_override_velocity(ply, (vec2){ply->velocity[0], -PLAYER_JUMP_FORCE});

        if(ply->velocity[0] == 0) play_animation(&ply->animator_component, &ANIM_PLAYER_IDLE);
        else if(ply->is_grounded) play_animation(&ply->animator_component, &ANIM_PLAYER_RUN);
    }
    else play_animation(&ply->animator_component, &ANIM_PLAYER_JUMP);

    // basic movement

    if(global_input.keysPressed[SAPP_KEYCODE_D]) ply->velocity[0] = PLAYER_MAX_SPEED;
    else if(global_input.keysPressed[SAPP_KEYCODE_A]) ply->velocity[0] = -PLAYER_MAX_SPEED;
    else ply->velocity[0] = 0;

    // look right or left dependent on where the mouse is of the player center

    if(global_input.mouse_x >= ply->position[0] + (ply->hit_box[0] / 2) + ply->hit_box_offset[0]) ply->sprite_data.flip_x = false;
    else ply->sprite_data.flip_x = true;

    // debug

    if(global_input.keysPressed[SAPP_KEYCODE_E]) camera_shake(5.0f);
}

void player_reset()
{

}