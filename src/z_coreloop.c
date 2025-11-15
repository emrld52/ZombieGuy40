#include "r_renderfuncs.h"
#include "z_coreloop.h"
#include "z_player.h"
#include "g_state.h"
#include "z_zombies.h"
#include "s_tilemap.h"
#include "s_scene.h"
#include "s_entities.h"
#include "s_animation.h"

// libs

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include <string.h>
#include <math.h>

// init global camera

vec2 global_camera_position;

scene loaded_scenes[MAX_LOADED_SCENES];
scene *loaded_scene;

vec2 heart_positions[3];
float time;

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

    animation_load_animations();

    player_init();

    // fill tilemap with basic floor for now, hardcode a level

    for(int y = 0; y < LEVELS_HEIGHT; y++)
    {
        for(int x = 0; x < LEVELS_WIDTH; x++)
        {
            if(y < 14) loaded_scene->tilemap[y][x].is_filled = false;
            else loaded_scene->tilemap[y][x].is_filled = true;
        }
    }

    loaded_scene->tilemap[10][6].is_filled = true;
    loaded_scene->tilemap[10][7].is_filled = true;
    loaded_scene->tilemap[10][8].is_filled = true;

    for(int y = 12; y < 16; y++)
    {
        for(int x = 0; x < 4; x++)
        {
            loaded_scene->tilemap[y][x].is_filled = true;
        }
    }

    loaded_scene->tilemap[10][19 - 6].is_filled = true;
    loaded_scene->tilemap[10][19 - 7].is_filled = true;
    loaded_scene->tilemap[10][19 - 8].is_filled = true;

    for(int y = 12; y < 16; y++)
    {
        for(int x = 16; x < 20; x++)
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
                animator_update(&loaded_scene->entities[i].animator_component);
                animator_get_frame(&loaded_scene->entities[i].animator_component, &loaded_scene->entities[i].sprite_data);
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

        draw_call((sprite)
        {
            .resolution[0] = 130, .resolution[1] = 32,
            .sprite_coord[0] = 17, .sprite_coord[1] = 2,
            .pos[0] = 4, .pos[1] = 4,
            .ui = true
        });

        // debug bouncy hearts UI

        time += global_delta_time * 5;

        for(int i = 0; i < 3; i++)
        {
            heart_positions[i][1] = sin(time + i * 0.5f) * 2.5f;

            draw_call((sprite)
            {
                .resolution[0] = 32, .resolution[1] = 32,
                .sprite_coord[0] = 20, .sprite_coord[1] = 1,
                .pos[0] = 8 + (32 * i) + (8 * i), .pos[1] = 4 + heart_positions[i][1],
                .ui = true
            });
        }

        draw_call((sprite)
        {
            .resolution[0] = 32, .resolution[1] = 32,
            .sprite_coord[0] = 19, .sprite_coord[1] = 1,
            .pos[0] = global_input.mouse_x - 16, .pos[1] = global_input.mouse_y - 16,
            .ui = true
        });

        for(int i = 0; i < 512; i++)
        {
            global_input.keys_released[i] = false;
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
                global_input.keys_pressed[ev->key_code] = true;
                global_input.key_tracker[ev->key_code] = false;
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            if (ev->key_code < 512) {
                global_input.keys_pressed[ev->key_code] = false;
                if(!global_input.key_tracker[ev->key_code]) {
                    global_input.keys_released[ev->key_code] = true;
                    global_input.key_tracker[ev->key_code] = true;
                }
            }
            break;

        case SAPP_EVENTTYPE_MOUSE_ENTER:
            sapp_show_mouse(false);
            break;

        case SAPP_EVENTTYPE_MOUSE_LEAVE:
            sapp_show_mouse(true);
            break;

        default:
            
            break;
    }

    global_input.mouse_x = ev->mouse_x;
    global_input.mouse_y = ev->mouse_y;
}

void program_loop()
{
    run_gameloop();
    draw_game();
}

void program_cleanup()
{
    sapp_show_mouse(true);
    sg_shutdown();
}