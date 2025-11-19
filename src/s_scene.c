// libs

#include "../deps/cglm/cglm.h"
#include <string.h>

// source

#include "s_scene.h"
#include "s_entities.h"
#include "g_state.h"

// ensure you include what you need for animator/sprite reset
void reset_entity(entity* ent)
{
    if(!ent) return;

    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        entity* e = &loaded_scene->entities[i];
        if(!e->enabled) continue;

        // Remove from colliding list
        for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
        {
            if(e->colliding_entities[z] == ent)
                e->colliding_entities[z] = NULL;
        }
    }

    // reset the arrays
    memset(ent->colliding_entities, 0, sizeof(ent->colliding_entities));

    *ent = (entity){0};

    ent->enabled = false;
    ent->collision_enabled = true;
    ent->gravity = 0.0f;
    ent->health_points = 0;
    ent->damage = 0;
    ent->team = 0;
    ent->id = 0;
    ent->handle_x_for_me = false;
    ent->is_grounded = false;
    ent->is_colliding = false;
    ent->is_projectile = false;
    ent->entity_timer = 0.0f;
    ent->marked_for_garbage_collection = false;

    ent->position[0] = ent->position[1] = 0.0f;
    ent->velocity[0] = ent->velocity[1] = 0.0f;
    ent->hit_box[0] = ent->hit_box[1] = 0.0f;
    ent->hit_box_offset[0] = ent->hit_box_offset[1] = 0.0f;

    ent->sprite_data = (sprite){0};
    ent->animator_component = (animator){0};
}

entity* make_entity_in_scene(scene *scn)
{
    // prefer to find an entirely disabled entity as opposed to one marked for garbage collection. this is so bodies and such linger around

    int optional_free_slot = -1;

    // linear search for available entity to provide from object pool

    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        if(scn->entities[i].enabled != true) 
        {
            if(&scn->entities[i] != NULL) reset_entity(&scn->entities[i]);

            // mark as now in use

            scn->entities[i].enabled = true;
            scn->entities[i].collision_enabled = true;
            
            return &scn->entities[i];
        }

        if(scn->entities[i].marked_for_garbage_collection == true && optional_free_slot == -1) 
        {
            optional_free_slot = i;
        }
    }

    if(optional_free_slot != -1)
    {
        if(&scn->entities[optional_free_slot] != NULL) reset_entity(&scn->entities[optional_free_slot]);

        // mark as now in use

        scn->entities[optional_free_slot].enabled = true;
        scn->entities[optional_free_slot].collision_enabled = true;
        
        return &scn->entities[optional_free_slot];
    }

    return NULL;
}

void destroy_entity_in_scene(entity *ent)
{
    ent->collision_enabled = false;
    ent->enabled = false;
}

// reset only entity, projectile and camera fields. its likely that things like level layout we want to keep

void reset_scene(scene *scn)
{
    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        scn->entities[i].enabled = false;
        scn->entities[i] = (entity){0};
    }

    glm_vec2_copy((vec2){0, 0}, scn->scene_camera_position);
}