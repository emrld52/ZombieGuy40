
#include "z_player.h"

// global state

#include "g_state.h"

// cglm gl math

#include "../deps/cglm/cglm.h"

// rendering, might do this in core loop, not sure right now. player pos currently not exposed

#include "r_renderfuncs.h"

static vec3 pos;
static vec2 vel;

static sprite p;

// init player position and velocity

void player_init()
{
    for(int i = 0; i < 3; i++)
    {
        pos[i] = 0.0f;
        vel[i] = 0.0f;
    }
}

void player_loop()
{
    glm_vec3_copy(pos, p.pos);

    draw_call(p);
}

void player_reset()
{

}