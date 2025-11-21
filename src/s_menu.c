#include "r_renderfuncs.h"
#include "s_animation.h"
#include "g_state.h"
#include "z_coreloop.h"
#include "s_text_renderer.h"

#include "../deps/sokol_app.h"

animator slip1;
animator slip2;
sprite slip_sprite1;
sprite slip_sprite2;

float pos;
float tme = 0;

bool already_hovered1 = false;
bool already_hovered2 = false;

void init_main_menu()
{
    slip_sprite1 = (sprite) {
        .sprite_coord[0] = 10,
        .sprite_coord[1] = 1,
        .resolution[0] = 128 + 32,
        .resolution[1] = 32,
        .pos[0] = (VIRTUAL_WIDTH / 2) - ((128 + 32) / 2) - 16,
        .pos[1] = 350 - 8
    };

    slip_sprite2 = (sprite) {
        .sprite_coord[0] = 10,
        .sprite_coord[1] = 1,
        .resolution[0] = 128 + 32,
        .resolution[1] = 32,
        .pos[0] = (VIRTUAL_WIDTH / 2) - ((128 + 32) / 2) - 16,
        .pos[1] = 350 - 8 + 24
    };

    animator_init(&slip1);
    animator_init(&slip2);

    play_animation(&slip1, &ANIM_STRIP);
    play_animation(&slip2, &ANIM_STRIP);
}

void draw_main_menu()
{
    tme += global_delta_time * loaded_scene->scene_game_speed;
    pos = sin(tme * 2) * 10;

    draw_call((sprite) {
        .sprite_coord[0] = 21,
        .sprite_coord[1] = 1,
        .resolution[0] = 640,
        .resolution[1] = 280,
        .pos[0] = 32 * 3.1f,
        .pos[1] = 55 + pos
    });

    //play_override_animation(&slip1, ANIM_STRIP_FLASH);

    draw_call(slip_sprite1);
    draw_call(slip_sprite2);

    render_text("play game", 10, (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350});

    play_animation(&slip1, &ANIM_STRIP);
    play_animation(&slip2, &ANIM_STRIP);

    if(!is_point_within_text((vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 }, 10, (vec2){global_input.mouse_x, global_input.mouse_y})) {
        already_hovered1 = false;
    }
    else
    {
        play_animation(&slip1, &ANIM_STRIP_HIGHLIGHTED);
        if(!already_hovered1) 
        {
            play_override_animation(&slip1, ANIM_STRIP_FLASH);
            camera_shake(1.0f);
        }
        already_hovered1 = true;
        if(global_input.mouse_l_up) 
        {
            load_scene(0);
            camera_shake(30.0f);
        }
    }

    if(!is_point_within_text((vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 + 24 }, 10, (vec2){global_input.mouse_x, global_input.mouse_y})) {
        already_hovered2 = false;
    }
    else {
        play_animation(&slip2, &ANIM_STRIP_HIGHLIGHTED);
        if(!already_hovered2) 
        {
            play_override_animation(&slip2, ANIM_STRIP_FLASH);
            camera_shake(1.0f);
        }
        already_hovered2 = true;
        if(global_input.mouse_l_up) sapp_quit();
    }

    animator_update(&slip1);
    animator_update(&slip2);

    animator_get_frame(&slip1, &slip_sprite1);
    animator_get_frame(&slip2, &slip_sprite2);

    render_text("quit game", 10, (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 + 24});
}