
#include "z_player.h"

// global state

#include "g_state.h"

// cglm gl math

#include "../deps/cglm/cglm.h"

// sokol

#include "../deps/sokol_app.h"

// rendering, might do this in core loop, not sure right now. player pos currently not exposed

#include "r_renderfuncs.h"

static vec3 pos;
static vec2 vel;

static sprite p;

// init player position and velocity

void player_init()
{
    glm_vec3_copy((vec3){ 0.0f, 0.0f, 0.0f }, pos);
    glm_vec3_copy((vec3){ 0.0f, 0.0f, 0.0f }, vel);
}

void player_loop()
{
    glm_vec3_copy(pos, p.pos);

    if(global_input.keysPressed[SAPP_KEYCODE_D]) pos[0] += 0.1f;

    draw_call(p);
}

void player_reset()
{

}