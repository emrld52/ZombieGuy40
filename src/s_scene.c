// libs

#include "../deps/cglm/cglm.h"

// source

#include "s_scene.h"
#include "s_entities.h"

entity* make_entity_in_scene(scene *scn)
{
    // linear search for available entity to provide from object pool

    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        if(scn->entities[i].enabled != true) 
        {
            // mark as now in use

            scn->entities[i].enabled = true;
            
            return &scn->entities[i];
        }
    }
}

void destroy_entity_in_scene(entity *ent)
{
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