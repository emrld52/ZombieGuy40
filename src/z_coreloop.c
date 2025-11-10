#include "r_renderfuncs.h"
#include "z_coreloop.h"
#include "z_player.h"
#include "g_state.h"
#include "z_zombies.h"

// libs

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"

// init global camera

vec2 global_camera_position;

void camera_shake(float magnitude)
{
    float angle = ((float)rand() / (float)RAND_MAX) * 2.0f * M_PI;
    vec2 dir; 
    dir[0] = cosf(angle) * magnitude;
    dir[1] = sinf(angle) * magnitude;

    glm_vec2_add(global_camera_position, dir, global_camera_position);
}

void gameloop_init()
{
    global_camera_position[0] = 0;
    global_camera_position[1] = 0;

    player_init();

    spawn_zombie(0, 1, 48.0f, 512.0f);
}

void run_gameloop()
{
    player_loop();

    simulate_zombies(p);

    render_zombies();

    // smooth camera back to 0, 0. shake will automatically resolve to 0, 0

    glm_vec2_lerp(global_camera_position, GLM_VEC2_ZERO, 8 * global_delta_time, global_camera_position);
}