// THIS SCRIPT ACHIEVES A PARALLAX BACKGROUND WITH SOME FURTHER FX

// source

#include "r_renderfuncs.h"
#include "r_backgroundfx.h"
#include "g_state.h"

sprite parallax_bg;
sprite parallax_bg2;
sprite parallax_bg3;
sprite bg;
sprite bg2;
sprite tornado;

float anim_time = 0.0f;

void init_background_fx()
{
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
}

void draw_background_fx()
{
    anim_time += global_delta_time * loaded_scene->scene_game_speed;

    parallax_bg.sprite_offset[0] += 0.07f * global_delta_time * loaded_scene->scene_game_speed;
    parallax_bg2.sprite_offset[0] += 0.1f * global_delta_time * loaded_scene->scene_game_speed;
    bg2.sprite_offset[0] += 0.035f * global_delta_time * loaded_scene->scene_game_speed;

    tornado.pos[0] = sin(anim_time * 0.5f) * 2.5f;

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
}