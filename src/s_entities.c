#include <stdbool.h>

#include "s_entities.h"
#include "s_scene.h"
#include "g_state.h"

void entity_run_physics(entity* ent)
{
    // if in the air, begin applying gravity, if grounded y velocity = 0. for now ground is just the bottom of the window, no collision yet.

    if(ent->position[1] + ent->hit_box[1] < 448) 
    {
        ent->is_grounded = false;
        ent->velocity[1] += ent->gravity * global_delta_time * loaded_scene->scene_game_speed;
    }
    else 
    {
        // if frame timing led to dropping slightly through floor adjust the next frame after

        if(ent->position[1] + ent->hit_box[1] >= 448) ent->position[1] = 448 - ent->hit_box[1];

        ent->is_grounded = true;
        if(ent->velocity[1] >= 0) ent->velocity[1] = 0;
    }

    // apply velocity scaled by delta time

    vec2 delta;
    glm_vec2_scale(ent->velocity,
                global_delta_time * loaded_scene->scene_game_speed,
                delta);

    glm_vec2_add(ent->position, delta, ent->position);
    glm_vec2_copy(ent->position, ent->sprite_data.pos);
}

void entity_override_velocity(entity* ent, vec2 vel)
{
    glm_vec2_copy(vel, ent->velocity);
}