#include "r_renderfuncs.h"
#include "z_coreloop.h"
#include "z_player.h"
#include "g_state.h"
#include "z_zombies.h"
#include "s_tilemap.h"
#include "s_scene.h"
#include "s_entities.h"

// libs

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include <string.h>

// init global camera

vec2 global_camera_position;

scene loaded_scenes[MAX_LOADED_SCENES];
scene *loaded_scene;

void gameloop_init()
{
    sapp_dpi_scale();
    // make scene

    loaded_scenes[0] = (scene)
    {
        .scene_game_speed = 1.0f,
        .type = SCENE_TYPE_LEVEL
    };

    loaded_scene = &loaded_scenes[0];

    // init camera pos

    global_camera_position[0] = 0;
    global_camera_position[1] = 0;

    player_init();

    // fill tilemap with basic floor for now

    for(int y = 0; y < 15; y++)
    {
        for(int x = 0; x < 20; x++)
        {
            if(y < 14) loaded_scene->tilemap[y][x].is_filled = false;
            else loaded_scene->tilemap[y][x].is_filled = true;
        }
    }

    loaded_scene->tilemap[8][4].is_filled = true;
    loaded_scene->tilemap[8][5].is_filled = true;
    loaded_scene->tilemap[8][6].is_filled = true;

    for(int y = 11; y < 15; y++)
    {
        for(int x = 5; x < 9; x++)
        {
            loaded_scene->tilemap[y][x].is_filled = true;
        }
    }

    init_tilemap(loaded_scene->tilemap);
    autotiler_build_tilemap(loaded_scene->tilemap);
}

void run_gameloop()
{
    switch(loaded_scene->type)
    {
    // scene is a menu, no menu yet so this is whatever
    case 0:

    break;

    // scene is a level
    case 1:
        player_loop();

        for(int i = 0; i < MAX_ENTITIES; i++)
        {
            if(loaded_scene->entities[i].enabled)
            {
                entity_run_physics(&loaded_scene->entities[i]);
                draw_call(loaded_scene->entities[i].sprite_data);
            }
        }

        // render tiles

        for(int y = 0; y < LEVELS_HEIGHT; y++)
        {
            for(int x = 0; x < LEVELS_WIDTH; x++)
            {
                if(loaded_scene->tilemap[y][x].is_filled == true)
                {
                    draw_call(loaded_scene->tilemap[y][x].tile_sprite);
                }
            }
        }

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