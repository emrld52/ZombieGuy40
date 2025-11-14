#include "r_renderfuncs.h"
#include "z_coreloop.h"
#include "z_player.h"
#include "g_state.h"
#include "z_zombies.h"
#include "s_tilemap.h"

// libs

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include <string.h>

// init global camera

vec2 global_camera_position;

tile tilemap[15][20];

int global_game_state = 1;

void gameloop_init()
{
    // init camera pos

    global_camera_position[0] = 0;
    global_camera_position[1] = 0;

    player_init();

    // fill tilemap with basic floor for now

    for(int y = 0; y < 15; y++)
    {
        for(int x = 0; x < 20; x++)
        {
            if(y < 14) tilemap[y][x].is_filled = false;
            else tilemap[y][x].is_filled = true;
        }
    }

    init_tilemap(tilemap);
    autotiler_build_tilemap(tilemap);
}

void run_gameloop()
{
    switch(global_game_state)
    {
    case 0:
    break;

    case 1:
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

        // render tiles

        for(int y = 0; y < 20; y++)
        {
            for(int x = 0; x < 20; x++)
            {
                if(tilemap[y][x].is_filled == true)
                {
                    draw_call(tilemap[y][x].tile_sprite);
                }
            }
        }

        // smooth camera back to 0, 0. shake will automatically resolve to 0, 0

        glm_vec2_lerp(global_camera_position, GLM_VEC2_ZERO, 8 * global_delta_time, global_camera_position);

    break;
    }
}

// init input

input_reader global_input = { 0 };
float global_game_speed = 1.0f;

void program_init(void)
{ 
    memset(&global_input, 0, sizeof(input_reader));

    gameloop_init();
    init_rendering();
}

// pass input into a globally accessible input state, used as event callback by sokol

void program_event(const sapp_event* ev) {
    switch (ev->type) {
        case SAPP_EVENTTYPE_KEY_DOWN:
            if (ev->key_code < 512) {
                global_input.keysPressed[ev->key_code] = true;
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            if (ev->key_code < 512) {
                global_input.keysPressed[ev->key_code] = false;
            }
            break;

        default:
            
            break;
    }
}

void program_loop()
{
    run_gameloop();
    draw_game();
}

void program_cleanup()
{
    sg_shutdown();
}