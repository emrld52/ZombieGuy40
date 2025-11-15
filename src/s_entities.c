#include <stdbool.h>

#include "s_entities.h"
#include "s_scene.h"
#include "g_state.h"
#include "r_renderfuncs.h"

void entity_run_physics(entity* ent)
{
    ent->is_grounded = false;
    ent->velocity[1] += ent->gravity * global_delta_time * loaded_scene->scene_game_speed;

    // define bounds for collision box, do one for x velocity and y velocity as to keep collisions axis independant to not get sticking when pushing into walls while falling

    vec2 ent_box_x_check[2];
    glm_vec2_copy((vec2){
        ent->position[0] + ent->hit_box_offset[0] + ent->velocity[0] * global_delta_time * loaded_scene->scene_game_speed,
        ent->position[1] + ent->hit_box_offset[1]
    }, ent_box_x_check[0]);

    glm_vec2_copy((vec2){
        ent->position[0] + ent->hit_box_offset[0] + ent->hit_box[0] + ent->velocity[0] * global_delta_time * loaded_scene->scene_game_speed,
        ent->position[1] + ent->hit_box_offset[1] + ent->hit_box[1]
    }, ent_box_x_check[1]);


    vec2 ent_box_y_check[2];
    glm_vec2_copy((vec2){
        ent->position[0] + ent->hit_box_offset[0],
        ent->position[1] + ent->hit_box_offset[1] + ent->velocity[1] * global_delta_time * loaded_scene->scene_game_speed
    }, ent_box_y_check[0]);

    glm_vec2_copy((vec2){
        ent->position[0] + ent->hit_box_offset[0] + ent->hit_box[0],
        ent->position[1] + ent->hit_box_offset[1] + ent->hit_box[1] + ent->velocity[1] * global_delta_time * loaded_scene->scene_game_speed
    }, ent_box_y_check[1]);

    vec2 to_collide[2];

    bool x_colliding = false;
    bool y_colliding = false;

    for(int y = 0; y < LEVELS_HEIGHT; y++)
    {
        for(int x = 0; x < LEVELS_WIDTH; x++)
        {
            if(loaded_scene->tilemap[y][x].is_filled) 
            {
                glm_vec2_copy((vec2){x * LEVELS_TILE_RESOLUTION, y * LEVELS_TILE_RESOLUTION}, to_collide[0]);
                glm_vec2_copy((vec2){(x * LEVELS_TILE_RESOLUTION) + LEVELS_TILE_RESOLUTION, (y * LEVELS_TILE_RESOLUTION) + LEVELS_TILE_RESOLUTION}, to_collide[1]);
                
                if(glm_aabb2d_aabb(ent_box_x_check, to_collide)) 
                {
                    x_colliding = true;

                    // snapping to prevent slow gliding upon being about to collide

                    //if(ent->velocity[0] > 0) ent->position[0] = to_collide[0][0] - (ent->hit_box[0] + ent->hit_box_offset[0]) - 0.01f;
                    //else if(ent->velocity[0] < 0) ent->position[0] = to_collide[1][0] - ent->hit_box_offset[0] + 0.01f;
                }
                if(glm_aabb2d_aabb(ent_box_y_check, to_collide)) 
                {
                    y_colliding = true;

                    // snapping to prevent slow gliding upon being about to collide

                    //if(ent->velocity[1] > 0) ent->position[1] = to_collide[0][1] - (ent->hit_box[1] + ent->hit_box_offset[1]) - 0.01f;
                    //else if(ent->velocity[1] < 0) ent->position[1] = to_collide[1][1] + 0.01f;
                }
            }
        }
    }

    if(!x_colliding) ent->position[0] += ent->velocity[0] * global_delta_time * loaded_scene->scene_game_speed;
    if(!y_colliding) ent->position[1] += ent->velocity[1] * global_delta_time * loaded_scene->scene_game_speed;
    else if(ent->velocity[1] >= 0)
    {
        ent->is_grounded = true;
        ent->velocity[1] = 0;
    }
    else 
    {
        ent->is_grounded = false;
        ent->velocity[1] = 0;
    }

    glm_vec2_copy(ent->position, ent->sprite_data.pos);
}

void entity_override_velocity(entity* ent, vec2 vel)
{
    glm_vec2_copy(vel, ent->velocity);
}