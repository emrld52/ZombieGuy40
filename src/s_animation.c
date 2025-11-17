#include "s_animation.h"
#include "g_state.h"

#include "../deps/cglm/cglm.h"

animation ANIM_PLAYER_IDLE;
animation ANIM_PLAYER_RUN;
animation ANIM_PLAYER_JUMP;
animation ANIM_PLAYER_DAMAGE;

animation ANIM_MINION_IDLE;
animation ANIM_MINION_RUN;
animation ANIM_MINION_JUMP;

animation ANIM_RANGER_IDLE;
animation ANIM_RANGER_RUN;
animation ANIM_RANGER_JUMP;

animation ANIM_KING_IDLE;
animation ANIM_KING_RUN;
animation ANIM_KING_JUMP;

animation ANIM_ZOMBIE_DAMAGE;

animation ANIM_ZOMBIE_DEAD1;
animation ANIM_ZOMBIE_DEAD2;

animation ANIM_BULLET_DEAFULT_FLASH;
animation ANIM_BULLET_DEAFULT_ZOOM;

animation ANIM_BULLET_ENEMY_FLASH;
animation ANIM_BULLET_ENEMY_ZOOM;

animation ANIM_HEART_TOOK_DAMAGE;
animation ANIM_HEART;
animation ANIM_HEART_BROKEN;

animation ANIM_CRATE_FALLING;
animation ANIM_CRATE_LANDED;
animation ANIM_CRATE_FLASH;

// unsure of how to store all animations to be referenced throughout the program. this isnt the cleane

void animation_load_animations()
{
    ANIM_PLAYER_IDLE = (animation) {
        .id = 0,
        .length_in_frames = 4,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 1},
        }
    };

    ANIM_PLAYER_RUN = (animation) {
        .id = 1,
        .length_in_frames = 5,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 2},
            {2, 2},
            {3, 2},
            {4, 2},
            {5, 2},
        }
    };

    ANIM_PLAYER_JUMP = (animation) {
        .id = 2,
        .length_in_frames = 6,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 3},
            {2, 3},
            {3, 3},
            {4, 3},
            {5, 3},
            {6, 3},
        }
    };

    ANIM_PLAYER_DAMAGE = (animation) {
        .id = 4,
        .length_in_frames = 8,
        .dist_between_frames = 0.025f,
        .frames = {
            {1, 4},
            {2, 4},
            {3, 4},
            {4, 4},
            {5, 4},
            {6, 4},
            {7, 4},
            {8, 4},
        }
    };

    ANIM_MINION_IDLE = (animation) {
        .id = 5,
        .length_in_frames = 4,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 5},
            {2, 5},
            {3, 5},
            {4, 5},
        }
    };

    ANIM_MINION_RUN = (animation) {
        .id = 6,
        .length_in_frames = 5,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 6},
            {2, 6},
            {3, 6},
            {4, 6},
            {5, 6},
        }
    };

    ANIM_MINION_JUMP = (animation) {
        .id = 7,
        .length_in_frames = 6,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 7},
            {2, 7},
            {3, 7},
            {4, 7},
            {5, 7},
            {6, 7},
        }
    };

    ANIM_RANGER_IDLE = (animation) {
        .id = 8,
        .length_in_frames = 4,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 8},
            {2, 8},
            {3, 8},
            {4, 8},
        }
    };

    ANIM_RANGER_RUN = (animation) {
        .id = 9,
        .length_in_frames = 5,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 9},
            {2, 9},
            {3, 9},
            {4, 9},
            {5, 9},
        }
    };

    ANIM_RANGER_JUMP = (animation) {
        .id = 10,
        .length_in_frames = 6,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 10},
            {2, 10},
            {3, 10},
            {4, 10},
            {5, 10},
            {6, 10},
        }
    };

    ANIM_KING_IDLE = (animation) {
        .id = 11,
        .length_in_frames = 4,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 11},
            {2, 11},
            {3, 11},
            {4, 11},
        }
    };

    ANIM_KING_RUN = (animation) {
        .id = 12,
        .length_in_frames = 5,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 12},
            {2, 12},
            {3, 12},
            {4, 12},
            {5, 12},
        }
    };

    ANIM_KING_JUMP = (animation) {
        .id = 13,
        .length_in_frames = 6,
        .dist_between_frames = 0.1f,
        .frames = {
            {1, 13},
            {2, 13},
            {3, 13},
            {4, 13},
            {5, 13},
            {6, 13},
        }
    };

    ANIM_ZOMBIE_DAMAGE = (animation) {
        .id = 14,
        .length_in_frames = 8,
        .dist_between_frames = 0.025f,
        .frames = {
            {1, 14},
            {2, 14},
            {3, 14},
            {4, 14},
            {5, 14},
            {6, 14},
            {7, 14},
            {8, 14},
        }
    };

    ANIM_ZOMBIE_DEAD1 = (animation) {
        .id = 15,
        .length_in_frames = 1,
        .dist_between_frames = 1.0f,
        .frames = {
            {1, 19}
        }
    };

    ANIM_ZOMBIE_DEAD2 = (animation) {
        .id = 15,
        .length_in_frames = 1,
        .dist_between_frames = 1.0f,
        .frames = {
            {2, 19}
        }
    };

    ANIM_BULLET_DEAFULT_FLASH = (animation) {
        .id = 16,
        .length_in_frames = 3,
        .dist_between_frames = 0.05f,
        .frames = {
            {16, 19},
            {17, 19},
            {18, 19}
        }
    };

    ANIM_BULLET_DEAFULT_ZOOM = (animation) {
        .id = 17,
        .length_in_frames = 5,
        .dist_between_frames = 0.05f,
        .frames = {
            {16, 20},
            {17, 20},
            {18, 20},
            {19, 20},
            {20, 20}
        }
    };

    ANIM_BULLET_ENEMY_FLASH = (animation) {
        .id = 18,
        .length_in_frames = 3,
        .dist_between_frames = 0.05f,
        .frames = {
            {16, 17},
            {17, 17},
            {18, 17}
        }
    };

    ANIM_BULLET_ENEMY_ZOOM = (animation) {
        .id = 19,
        .length_in_frames = 5,
        .dist_between_frames = 0.05f,
        .frames = {
            {16, 18},
            {17, 18},
            {18, 18},
            {19, 18},
            {20, 18}
        }
    };

    ANIM_HEART = (animation) {
        .id = 20,
        .length_in_frames = 1,
        .dist_between_frames = 0.05f,
        .frames = {
            {20, 1}
        }
    };

    ANIM_HEART_TOOK_DAMAGE = (animation) {
        .id = 21,
        .length_in_frames = 6,
        .dist_between_frames = 0.05f,
        .frames = {
            {15, 3},
            {16, 3},
            {17, 3},
            {18, 3},
            {19, 3},
            {20, 3}
        }
    };

    ANIM_HEART_BROKEN = (animation) {
        .id = 22,
        .length_in_frames = 1,
        .dist_between_frames = 0.05f,
        .frames = {
            {20, 4},
        }
    };

    ANIM_CRATE_FALLING = (animation) {
        .id = 23,
        .length_in_frames = 1,
        .dist_between_frames = 0.05f,
        .frames = {
            {14, 17},
        }
    };

    ANIM_CRATE_LANDED = (animation) {
        .id = 24,
        .length_in_frames = 1,
        .dist_between_frames = 0.05f,
        .frames = {
            {15, 17},
        }
    };

    ANIM_CRATE_FLASH = (animation) {
        .id = 24,
        .length_in_frames = 8,
        .dist_between_frames = 0.1f,
        .frames = {
            {15, 17},
            {13, 17},
            {15, 17},
            {13, 17},
            {15, 17},
            {13, 17},
            {15, 17},
            {13, 17},
        }
    };
}

void animator_init(animator *amtr)
{
    amtr->is_override_done = true;
    amtr->current_frame = 0;
    amtr->time_til_next_frame = 0;
    amtr->passive_animation.id = -1;
};

void animator_update(animator *amtr)
{
    amtr->time_til_next_frame -= global_delta_time * loaded_scene->scene_game_speed;

    if(amtr->time_til_next_frame <= 0) {
        if(!amtr->is_override_done)
        {
            if(amtr->current_frame + 1 > amtr->override_animation.length_in_frames - 1)
            {
                amtr->is_override_done = true;
                amtr->current_frame = 0;
                amtr->time_til_next_frame = amtr->passive_animation.dist_between_frames;
            }
            else 
            {
                amtr->current_frame += 1;
                amtr->time_til_next_frame = amtr->override_animation.dist_between_frames;
            }
        }
        else
        {
            if(amtr->current_frame + 1 > amtr->passive_animation.length_in_frames - 1) amtr->current_frame = 0;
            else amtr->current_frame += 1;
            amtr->time_til_next_frame = amtr->passive_animation.dist_between_frames;
        }
    }
}

void play_override_animation(animator *amtr, animation ovrd)
{
    amtr->is_override_done = false;
    amtr->current_frame = 0;
    amtr->override_animation = ovrd;
    amtr->time_til_next_frame = ovrd.dist_between_frames;
}

void play_animation(animator *amtr, animation *anim)
{
    //check not about to play same animation

    if(amtr->passive_animation.id != anim->id)
    {
        amtr->current_frame = 0;
        amtr->passive_animation = *anim;
        amtr->time_til_next_frame = anim->dist_between_frames;
    }
}

void animator_get_frame(animator *amtr, sprite *to_apply)
{
    vec2 *frames = amtr->is_override_done
        ? amtr->passive_animation.frames
        : amtr->override_animation.frames;

    int i = amtr->current_frame;

    to_apply->sprite_coord[0] = frames[i][0];
    to_apply->sprite_coord[1] = frames[i][1];
}
