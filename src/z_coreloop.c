#include "r_renderfuncs.h"
#include "z_coreloop.h"
#include "z_player.h"
#include "g_state.h"
#include "z_zombies.h"
#include "s_tilemap.h"
#include "s_scene.h"
#include "s_entities.h"
#include "s_animation.h"
#include "s_weapons.h"
#include "z_upgradecrate.h"
#include "s_playerUI.h"

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

sprite parallax_bg;
sprite parallax_bg2;
sprite parallax_bg3;
sprite bg;
sprite bg2;
sprite tornado;

sprite pause_icon;

bool is_paused = false;

float anim_time = 0.0f;

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
    
    init_weapon_system();

    player_init();

    init_hp_ui(get_player());

    // fill tilemap with basic floor for now, hardcode a level

    for(int y = 0; y < LEVELS_HEIGHT; y++)
    {
        for(int x = 0; x < LEVELS_WIDTH; x++)
        {
            if(y < 14) loaded_scene->tilemap[y][x].is_filled = false;
            else loaded_scene->tilemap[y][x].is_filled = true;
        }
    }

    loaded_scene->tilemap[12][8].is_sniping_position = true;

    loaded_scene->tilemap[10][6].is_filled = true;
    loaded_scene->tilemap[10][7].is_filled = true;
    loaded_scene->tilemap[10][8].is_filled = true;
    loaded_scene->tilemap[9][8].direction_for_pathing = 2;
    loaded_scene->tilemap[9][10].direction_for_pathing = 2;

    loaded_scene->tilemap[9][7].is_sniping_position = true;

    for(int y = 12; y < 16; y++)
    {
        for(int x = 0; x < 4; x++)
        {
            loaded_scene->tilemap[y][x].is_filled = true;
        }
    }

    loaded_scene->tilemap[11][2].is_sniping_position = true;

    loaded_scene->tilemap[10][19 - 6].is_filled = true;
    loaded_scene->tilemap[10][19 - 7].is_filled = true;
    loaded_scene->tilemap[10][19 - 8].is_filled = true;

    loaded_scene->tilemap[9][19 - 7].is_sniping_position = true;

    for(int y = 12; y < 16; y++)
    {
        for(int x = 16; x < 20; x++)
        {
            loaded_scene->tilemap[y][x].is_filled = true;
        }
    }

    loaded_scene->tilemap[11][18].is_sniping_position = true;

    loaded_scene->tilemap[13][3].direction_for_pathing = 2;
    loaded_scene->tilemap[11][2].direction_for_pathing = 3;
    loaded_scene->tilemap[11][3].direction_for_pathing = 1;
    loaded_scene->tilemap[13][15].direction_for_pathing = 2;
    loaded_scene->tilemap[11][16].direction_for_pathing = 4;
    loaded_scene->tilemap[11][15].direction_for_pathing = 1;

    init_tilemap(loaded_scene->tilemap);
    autotiler_build_tilemap(loaded_scene->tilemap);

    parallax_bg = (sprite)
    {
        .sprite_coord[0] = 21,
        .sprite_coord[1] = 30,
        .resolution[0] = 640,
        .resolution[1] = 352,
        .pos[1] = 100,
        .ui = true
    };

    parallax_bg2 = (sprite)
    {
        .sprite_coord[0] = 21,
        .sprite_coord[1] = 10,
        .resolution[0] = 640,
        .resolution[1] = 352,
        .pos[1] = 200,
        .ui = true
    };

    bg = (sprite)
    {
        .sprite_coord[0] = 1,
        .sprite_coord[1] = 30,
        .resolution[0] = 640,
        .resolution[1] = 352,
        .ui = true
    };

    tornado = (sprite)
    {
        .sprite_coord[0] = 1,
        .sprite_coord[1] = 20,
        .resolution[0] = 32*8,
        .resolution[1] = 32*9,
        .pos[0] = 0,
        .pos[1] = 16,
        .ui = true
    };

    bg2 = (sprite)
    {
        .sprite_coord[0] = 21,
        .sprite_coord[1] = 21,
        .resolution[0] = 640,
        .resolution[1] = 352,
        .ui = true
    };

    init_supply_crate();
    destroy_crate();

    pause_icon = (sprite)
    {
        .sprite_coord[0] = 15,
        .sprite_coord[1] = 1,
        .resolution[0] = 64,
        .resolution[1] = 64,
        .pos[0] = 640 - 72,
        .pos[1] = 8,
        .ui = true
    };
}

void run_gameloop()
{
    if(global_input.keys_released[SAPP_KEYCODE_ESCAPE]) is_paused = !is_paused;

    if(is_paused) 
    {
        loaded_scene->scene_game_speed = 0;
        draw_call(pause_icon);
    }
    else loaded_scene->scene_game_speed = 1;

    switch(loaded_scene->type)
    {
    // scene is a menu, no menu yet so this is whatever
    case 0:

    break;

    // scene is a level
    case 1:
        for(int i = 0; i < MAX_ENTITIES; i++)
        {
            if(loaded_scene->entities[i].enabled)
            {
                entity_run_physics(&loaded_scene->entities[i]);
                if(&loaded_scene->entities[i].animator_component != NULL) 
                {
                    animator_update(&loaded_scene->entities[i].animator_component);
                    animator_get_frame(&loaded_scene->entities[i].animator_component, &loaded_scene->entities[i].sprite_data);
                }
                draw_call(loaded_scene->entities[i].sprite_data);
            }
        }

        bullets_update();
        player_loop();
        simulate_zombies(get_player());

        update_supply_crate();

        cleanup_stale_ignore_lists();

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

        // debug spawn zom

        //if(global_input.keys_released[SAPP_KEYCODE_Z]) spawn_zombie(3, 1, 72.0f, 512.0f * 0.75f);

        // debug bouncy hearts UI

        anim_time += global_delta_time * 5;

        /*for(int i = 0; i < 3; i++)
        {
            heart_positions[i][1] = sin(time + i * 0.5f) * 2.5f;

            draw_call((sprite)
            {
                .resolution[0] = 32, .resolution[1] = 32,
                .sprite_coord[0] = 20, .sprite_coord[1] = 1,
                .pos[0] = 8 + (32 * i) + (8 * i), .pos[1] = 4 + heart_positions[i][1],
                .ui = true
            });
        }*/

        for(int i = 0; i < 512; i++)
        {
            global_input.keys_released[i] = false;
        }

        global_input.mouse_l_up = false;
        global_input.mouse_r_up = false;

        parallax_bg.sprite_offset[0] += 0.07f * global_delta_time * loaded_scene->scene_game_speed;
        parallax_bg2.sprite_offset[0] += 0.1f * global_delta_time * loaded_scene->scene_game_speed;
        bg2.sprite_offset[0] += 0.035f * global_delta_time * loaded_scene->scene_game_speed;

        tornado.pos[0] = sin(anim_time * 0.5f) * 2.5f;

        draw_hp_ui(get_player());

        first_priority_draw_call((sprite)
        {
            .resolution[0] = 130, .resolution[1] = 32,
            .sprite_coord[0] = 17, .sprite_coord[1] = 2,
            .pos[0] = 4, .pos[1] = 4,
            .ui = true
        });
        first_priority_draw_call(parallax_bg2);
        first_priority_draw_call(parallax_bg);
        first_priority_draw_call(tornado);
        first_priority_draw_call(bg2);
        first_priority_draw_call(bg);

    break;
    }

    draw_cursor();
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
        
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                global_input.mouse_l = true;
                global_input.mouse_l_tracker = false;
            }
            else if (ev->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
                global_input.mouse_r = true;
                global_input.mouse_r_tracker = false;
            }
            break;

        case SAPP_EVENTTYPE_MOUSE_UP:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                global_input.mouse_l = false;
                global_input.mouse_r = false;
                if(!global_input.mouse_l_up) {
                    global_input.mouse_l_up = true;
                    global_input.mouse_l_tracker = true;
                }
            }
            else if (ev->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
                global_input.mouse_r = false;
                if(!global_input.mouse_r_up) {
                    global_input.mouse_r_up = true;
                    global_input.mouse_r_tracker = true;
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

    global_input.mouse_x = (ev->mouse_x / sapp_width()) * VIRTUAL_WIDTH;
    global_input.mouse_y = (ev->mouse_y / sapp_height()) * VIRTUAL_HEIGHT;
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