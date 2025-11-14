// libs

#include "../deps/cglm/cglm.h"

// source

#include "r_renderfuncs.h"

typedef struct entity_t {
    vec2 position;
    vec2 velocity;
    vec2 hit_box;
    sprite sprite_data;
    bool enabled;
} entity;