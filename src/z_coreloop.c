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
#include "r_backgroundfx.h"
#include "s_input_handling.h"
#include "z_levels.h"

// test

#include "s_text_renderer.h"

// libs

#include "../deps/cglm/cglm.h"
#include "../deps/sokol_app.h"
#include "../deps/sokol_gfx.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

scene loaded_scenes[MAX_LOADED_SCENES];
scene *loaded_scene;

sprite pause_icon;

// init is_paused from game state

bool is_paused = false;

void gameloop_init()
{
    // make scene, DEBUG

    loaded_scenes[0] = (scene)
    {
        .scene_game_speed = 1.0f,
        .type = SCENE_TYPE_LEVEL
    };

    loaded_scenes[1] = (scene)
    {
        .scene_game_speed = 1.0f,
        .type = SCENE_TYPE_MENU
    };

    loaded_scene = &loaded_scenes[0];

    // FOLLOWING NEEDS TO BE IN ORDER

    animation_load_animations();

    // initializes bullet types
    
    init_weapon_system();

    // initializes a new player struct

    player_init();

    init_level_geometry();

    load_level_into_2d_array(0, loaded_scene->tilemap);

    init_tilemap(loaded_scene->tilemap);
    autotiler_build_tilemap(loaded_scene->tilemap);

    // init parallax background

    init_background_fx();

    init_text_renderer();

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

    loaded_scene = &loaded_scenes[0];
}

void run_gameloop()
{
    switch(loaded_scene->type)
    {
    // scene is a menu, no menu yet so this is whatever
    case SCENE_TYPE_MENU:

    break;

    // scene is a level
    case SCENE_TYPE_LEVEL:
        // pause logic
        
        if(global_input.keys_released[SAPP_KEYCODE_ESCAPE]) is_paused = !is_paused;

        if(is_paused) 
        {
            loaded_scene->scene_game_speed = 0;
            draw_call(pause_icon);
        }
        else loaded_scene->scene_game_speed = 1;

        draw_background_fx();

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

        player_loop();
        bullets_update();
        zombie_coreloop(get_player());

        update_supply_crate();

        // render tiles

        render_tilemap(loaded_scene->tilemap);

        // debug

        if(global_input.keys_released[SAPP_KEYCODE_R]) {
            reset_player(); 
            reset_zombie_progress();
            do_scene_garbage_collection(loaded_scene);
            init_supply_crate();
            destroy_crate();
        }

        free_released_keys();

        draw_hp_ui(get_player());

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

void program_loop()
{
    begin_drawing();
    run_gameloop();
    end_draw();
}

void program_cleanup()
{
    sapp_show_mouse(true);
    sg_shutdown();
}