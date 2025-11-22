#include "s_menu.h"
#include "r_renderfuncs.h"
#include "s_animation.h"
#include "g_state.h"
#include "z_coreloop.h"
#include "s_text_renderer.h"

#include "s_sound.h"

#include "../deps/sokol_app.h"

#include <string.h>

// MAIN MENU

button main_menu_btns[2];

// PAUSE MENU

button pause_menu_btns[2];

float pos;
float tme = 0;

bool is_button_clicked(button *btn, vec2 pos)
{
    draw_call(btn->button_strip);

    animator_update(&btn->button_animation);

    animator_get_frame(&btn->button_animation, &btn->button_strip);

    render_text(btn->txt, 10, pos);

    if(!is_point_within_text(pos, 10, (vec2){global_input.mouse_x, global_input.mouse_y})) {
        btn->was_hovered = false;
        play_animation(&btn->button_animation, &ANIM_STRIP);
    }
    else {
        play_animation(&btn->button_animation, &ANIM_STRIP_HIGHLIGHTED);
        if(!btn->was_hovered) 
        {
            play_override_animation(&btn->button_animation, ANIM_STRIP_FLASH);
            camera_shake(1.0f);
            play_sound("select_blip.wav");
        }
        btn->was_hovered = true;
        if(global_input.mouse_l_up) return true;
    }

    return false;
}

void init_menus()
{
    main_menu_btns[0].button_strip = (sprite) {
        .sprite_coord[0] = 10,
        .sprite_coord[1] = 1,
        .resolution[0] = 128 + 32,
        .resolution[1] = 32,
        .pos[0] = (VIRTUAL_WIDTH / 2) - ((128 + 32) / 2) - 16,
        .pos[1] = 350 - 8
    };

    main_menu_btns[1].button_strip = (sprite) {
        .sprite_coord[0] = 10,
        .sprite_coord[1] = 1,
        .resolution[0] = 128 + 32,
        .resolution[1] = 32,
        .pos[0] = (VIRTUAL_WIDTH / 2) - ((128 + 32) / 2) - 16,
        .pos[1] = 350 - 8 + 24
    };

    main_menu_btns[0].button_animation.not_affected_by_game_speed = true;
    main_menu_btns[1].button_animation.not_affected_by_game_speed = true;

    animator_init(&main_menu_btns[0].button_animation);
    animator_init(&main_menu_btns[1].button_animation);

    play_animation(&main_menu_btns[0].button_animation, &ANIM_STRIP);
    play_animation(&main_menu_btns[1].button_animation, &ANIM_STRIP);

    strcpy(main_menu_btns[0].txt, "play game");
    strcpy(main_menu_btns[1].txt, "quit game");

    pause_menu_btns[0].button_strip = (sprite) {
        .sprite_coord[0] = 10,
        .sprite_coord[1] = 1,
        .resolution[0] = 128 + 32,
        .resolution[1] = 32,
        .pos[0] = (VIRTUAL_WIDTH / 2) - ((128 + 32) / 2) - 16,
        .pos[1] = 350 - 8
    };

    pause_menu_btns[1].button_strip = (sprite) {
        .sprite_coord[0] = 10,
        .sprite_coord[1] = 1,
        .resolution[0] = 128 + 32,
        .resolution[1] = 32,
        .pos[0] = (VIRTUAL_WIDTH / 2) - ((128 + 32) / 2) - 16,
        .pos[1] = 350 - 8 + 24
    };

    pause_menu_btns[0].button_animation.not_affected_by_game_speed = true;
    pause_menu_btns[1].button_animation.not_affected_by_game_speed = true;

    animator_init(&pause_menu_btns[0].button_animation);
    animator_init(&pause_menu_btns[1].button_animation);

    play_animation(&pause_menu_btns[0].button_animation, &ANIM_STRIP);
    play_animation(&pause_menu_btns[1].button_animation, &ANIM_STRIP);

    strcpy(pause_menu_btns[0].txt, "resume");
    strcpy(pause_menu_btns[1].txt, "main menu");
}

void draw_main_menu()
{
    tme += global_delta_time * loaded_scene->scene_game_speed;
    pos = sin(tme * 2) * 10;

    // logo

    draw_call((sprite) {
        .sprite_coord[0] = 21,
        .sprite_coord[1] = 1,
        .resolution[0] = 640,
        .resolution[1] = 280,
        .pos[0] = 32 * 3.1f,
        .pos[1] = 55 + pos
    });

    if(is_button_clicked(&main_menu_btns[0], (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 })) {
        load_scene(0);
        reset_game_state();
    }
    else if(is_button_clicked(&main_menu_btns[1], (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 + 24})) sapp_quit();
}

void draw_pause_menu()
{
    if(is_button_clicked(&pause_menu_btns[0], (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 })) is_paused = false;
    else if(is_button_clicked(&pause_menu_btns[1], (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 + 24})) {
        is_paused = false;
        load_scene(1);
    }
}