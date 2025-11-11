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

sprite heart;
sprite text;
sprite half_bob;
sprite full_bob;

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

    heart = make_sprite(GLM_VEC2_ZERO, (vec2){20, 1}, true, (vec2){ 32, 32 });

    text = make_sprite((vec2) {0,64}, (vec2){1, 15}, true, (vec2){ 32 * 7, 32 });

    half_bob = make_sprite((vec2) {64,64}, (vec2){1, 16}, true, (vec2){ 32, 64 });
    full_bob = make_sprite((vec2) {0,128}, (vec2){1, 16}, true, (vec2){ 64, 64 });

    player_init();

    spawn_zombie(0, 1, 48.0f, 448.0f);
}

void run_gameloop()
{
    player_loop();

    simulate_zombies(p);

    render_zombies();

    // debug ui

    for(int i = 0; i < 3; i++)
    {
        draw_call((sprite){
            .sprite_coord[0] = 20,
            .sprite_coord[1] = 1,
            .pos[0] = 8 + (4 * i) + (32 * i),
            .pos[1] = 8,
            .ui = true,
            .resolution[0] = 32,
            .resolution[1] = 32
        }); 
    }

    draw_call(text);
    draw_call(full_bob);
    draw_call(half_bob);

    // screen shake test

    if(global_input.keysPressed[SAPP_KEYCODE_E]) spawn_zombie(0, 1, ((float)rand()/(float)(RAND_MAX/25.0f)) + 25.0f, ((float)rand()/(float)(RAND_MAX/256.0f)) + 256.0f);

    // smooth camera back to 0, 0. shake will automatically resolve to 0, 0

    glm_vec2_lerp(global_camera_position, GLM_VEC2_ZERO, 8 * global_delta_time, global_camera_position);
}