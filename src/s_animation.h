#pragma once

#include "r_renderfuncs.h"

#include <stdbool.h>
#include "../deps/cglm/cglm.h"

#define MAX_ANIMATION_FRAMES 8

typedef struct animation_t {
    int id;
    
    int length_in_frames;

    // in secs

    float dist_between_frames;

    // frames in texture atlas coordinates, saves memory
    
    vec2 frames[MAX_ANIMATION_FRAMES];
} animation;

// component that tracks what frame should currently be playing and controls animations

typedef struct animator_t {
    // looping animatiion to play such as running, idle or falling

    animation passive_animation;

    // slot for override animation such as damage flash or powerup gained

    animation override_animation;

    int current_frame;
    float time_til_next_frame;

    bool is_override_done;

    bool not_affected_by_game_speed;
} animator;

// animation list

extern animation ANIM_PLAYER_IDLE;
extern animation ANIM_PLAYER_RUN;
extern animation ANIM_PLAYER_JUMP;
extern animation ANIM_PLAYER_DAMAGE;

extern animation ANIM_MINION_IDLE;
extern animation ANIM_MINION_RUN;
extern animation ANIM_MINION_JUMP;

extern animation ANIM_RANGER_IDLE;
extern animation ANIM_RANGER_RUN;
extern animation ANIM_RANGER_JUMP;

extern animation ANIM_KING_IDLE;
extern animation ANIM_KING_RUN;
extern animation ANIM_KING_JUMP;

extern animation ANIM_ZOMBIE_DAMAGE;

extern animation ANIM_ZOMBIE_DEAD1;
extern animation ANIM_ZOMBIE_DEAD2;

extern animation ANIM_BULLET_DEAFULT_FLASH;
extern animation ANIM_BULLET_DEAFULT_ZOOM;

extern animation ANIM_BULLET_ENEMY_FLASH;
extern animation ANIM_BULLET_ENEMY_ZOOM;

extern animation ANIM_BULLET_BLUE_FLASH;
extern animation ANIM_BULLET_BLUE_ZOOM;

extern animation ANIM_BULLET_GREEN_FLASH;
extern animation ANIM_BULLET_GREEN_ZOOM;

extern animation ANIM_BULLET_THROWING_KNIFE;

extern animation ANIM_HEART_TOOK_DAMAGE;
extern animation ANIM_HEART;
extern animation ANIM_HEART_OVERHEAL;
extern animation ANIM_HEART_BROKEN;

extern animation ANIM_CRATE_FALLING;
extern animation ANIM_CRATE_LANDED;
extern animation ANIM_CRATE_FLASH;

extern animation ANIM_STRIP;
extern animation ANIM_STRIP_FLASH;
extern animation ANIM_STRIP_HIGHLIGHTED;

void animation_load_animations();

void animator_init(animator *amtr);
void animator_update(animator *amtr);

void play_override_animation(animator *amtr, animation ovrd);
void play_animation(animator *amtr, animation *anim);

void animator_get_frame(animator *amtr, sprite *to_apply);