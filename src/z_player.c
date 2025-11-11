
#include "z_player.h"

// global state

#include "g_state.h"

#include "z_coreloop.h"

// libraries

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include <stdbool.h>

// rendering, might do this in core loop, not sure right now. player pos currently not exposed

#include "r_renderfuncs.h"

// player-movmement variables

#define PLAYER_MAX_SPEED 256.0f
#define PLAYER_GRAVITY 1024.0f
#define PLAYER_JUMP_FORCE 512.0f

static vec3 pos;
static vec2 vel;

sprite p;

static bool is_grounded = false;

// init player position and velocity

void player_init()
{
    glm_vec3_copy((vec3){ (sapp_width() / 2) - 32, 0.0f, 0.0f }, pos);
    glm_vec3_copy((vec3){ 0.0f, 0.0f, 0.0f }, vel);

    p.sprite_coord[0] = 1;
    p.sprite_coord[1] = 1;
    p.resolution[0] = 32;
    p.resolution[1] = 32;
}

void player_loop()
{
    // if in the air, begin applying gravity, if grounded y velocity = 0. for now ground is just the bottom of the window, no collision yet.

    if(pos[1] + 32 < 480) 
    {
        is_grounded = false;
        vel[1] += PLAYER_GRAVITY * global_delta_time * global_game_speed;
    }
    else 
    {
        // if frame timing led to dropping slightly through floor adjust the next frame after

        if(pos[1] + 32 > 480) pos[1] = 480 - 32;

        is_grounded = true;
        vel[1] = 0;
    }

    if(is_grounded && global_input.keysPressed[SAPP_KEYCODE_SPACE]) vel[1] = -PLAYER_JUMP_FORCE;

    if(global_input.keysPressed[SAPP_KEYCODE_D]) vel[0] = PLAYER_MAX_SPEED;
    else if(global_input.keysPressed[SAPP_KEYCODE_A]) vel[0] = -PLAYER_MAX_SPEED;
    else vel[0] = 0;

    // screen shake test

    if(global_input.keysPressed[SAPP_KEYCODE_E] && global_camera_position[0] <= 0.25f)
    {
        camera_shake(5.0f);
    }

    // add velocity to position. first index (0) = x axis, second index = y

    pos[0] += vel[0] * global_delta_time * global_game_speed;
    pos[1] += vel[1] * global_delta_time * global_game_speed;

    // pass off calculated position into a sprite and send off to the draw queue

    glm_vec3_copy(pos, p.pos);

    draw_call(p);
}

void player_reset()
{

}