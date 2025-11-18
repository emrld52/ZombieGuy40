#include "s_playerUI.h"
#include "z_player.h"
#include "g_state.h"

// current heart count, can increase as player gains more max hp

int heart_count = 3;

animator heart[12];
sprite heart_sprite[12];

float time = 0;

void init_hp_ui(entity *plyr)
{
    heart_count = plyr->max_health_points;

    for(int i = 0; i < heart_count; i++)
    {
        animator_init(&heart[i]);

        heart_sprite[i] = (sprite)
        {
            .sprite_coord[0] = 21,
            .sprite_coord[1] = 21,
            .resolution[0] = 32,
            .resolution[1] = 32,
            .pos[0] = (10 + (38 * i)) % VIRTUAL_WIDTH, // offset each heart accordingly
            .pos[1] = 4,
            .ui = true
        };

        if(plyr->health_points > i) play_animation(&heart[i], &ANIM_HEART);
        else play_animation(&heart[i], &ANIM_HEART_BROKEN);
    }
}

void damage_ui_hp(entity *plyr)
{
    for(int i = heart_count - 1; i > get_player()->health_points - 1; i--) 
    {
        if(i >= 0) {
            if(heart[i].passive_animation.id != ANIM_HEART_BROKEN.id) play_override_animation(&heart[i], ANIM_HEART_TOOK_DAMAGE);
            play_animation(&heart[i], &ANIM_HEART_BROKEN);
        }
    }
}

void heal_ui_hp(entity *plyr)
{
    for(int i = 0; i < get_player()->health_points; i++) play_animation(&heart[i], &ANIM_HEART);
}

void draw_hp_ui()
{
    draw_call((sprite)
    {
        .resolution[0] = 6, .resolution[1] = 32,
        .sprite_coord[0] = 17, .sprite_coord[1] = 2,
        .pos[0] = 4, .pos[1] = 4,
        .ui = true
    });

    time += 5 * global_delta_time * loaded_scene->scene_game_speed;

    for(int i = 0; i < heart_count; i++) 
    {
        if(i != heart_count - 1) {
            draw_call((sprite)
            {
                .resolution[0] = 38, .resolution[1] = 32,
                .sprite_coord[0] = 18, .sprite_coord[1] = 2,
                .pos[0] = 10 + (38 * i), .pos[1] = 4,
                .ui = true
            });
        }
        else
        {
            draw_call((sprite)
            {
                .resolution[0] = 5, .resolution[1] = 32,
                .sprite_coord[0] = 18, .sprite_coord[1] = 2,
                .pos[0] = 10 + (38 * i), .pos[1] = 4,
                .ui = true
            });
            draw_call((sprite)
            {
                .resolution[0] = 32, .resolution[1] = 32,
                .sprite_coord[0] = 20, .sprite_coord[1] = 2,
                .pos[0] = 9 + (38 * i) + 6, .pos[1] = 4,
                .ui = true
            });
        }

        animator_update(&heart[i]);

        heart_sprite[i].pos[1] = (sin(time + i) * 2.5f) + 4;

        animator_get_frame(&heart[i], &heart_sprite[i]);
        draw_call(heart_sprite[i]);
    }
}

void draw_cursor()
{
    draw_call((sprite)
    {
        .resolution[0] = 32, .resolution[1] = 32,
        .sprite_coord[0] = 19, .sprite_coord[1] = 1,
        .pos[0] = global_input.mouse_x - 16, .pos[1] = global_input.mouse_y - 16,
        .ui = true
    });
}