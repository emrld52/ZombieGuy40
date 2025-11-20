#pragma once

// libs

#include "../deps/cglm/cglm.h"

// source

#include "s_entities.h"
#include "s_tilemap.h"

#define MAX_ENTITIES 256

typedef enum scene_type
{
    SCENE_TYPE_MENU  =  0,
    SCENE_TYPE_LEVEL =  1,
} scene_type;

typedef struct scene_t {
    scene_type type;

    // all scenes will have a tile map even though some scenes might not use them. however this data is only a few kilobytes

    tile tilemap[LEVELS_HEIGHT][LEVELS_WIDTH];
    entity entities[MAX_ENTITIES];

    vec2 scene_camera_position;

    int ticks_per_second;
    float time_til_tick;
    uint64_t tick_count;

    float scene_game_speed;
} scene;

entity* make_entity_in_scene(scene *scn);
void destroy_entity_in_scene(entity *ent);
void reset_entity(entity* ent);
void do_scene_garbage_collection(scene *scn);
void reset_scene(scene *scn);