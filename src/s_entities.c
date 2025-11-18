#include <stdbool.h>

#include "s_entities.h"
#include "s_scene.h"
#include "g_state.h"
#include "r_renderfuncs.h"

void entity_run_physics(entity* ent)
{
    ent->is_grounded = false;
    ent->velocity[1] += ent->gravity * global_delta_time * loaded_scene->scene_game_speed;

    if(ent->handle_x_for_me) ent->velocity[0] = glm_lerp(ent->velocity[0], 0, 2 * global_delta_time * loaded_scene->scene_game_speed);

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

    ent->is_colliding = false;

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
                    ent->is_colliding = true;

                    // snapping to prevent slow gliding upon being about to collide

                    //if(ent->velocity[0] > 0) ent->position[0] = to_collide[0][0] - (ent->hit_box[0] + ent->hit_box_offset[0]) - 0.01f;
                    //else if(ent->velocity[0] < 0) ent->position[0] = to_collide[1][0] - ent->hit_box_offset[0] + 0.01f;
                }
                else if(ent->position[0] + (ent->hit_box_offset[0] / 2) + (ent->velocity[0] * global_delta_time * loaded_scene->scene_game_speed) <= 0)
                {
                    x_colliding = true;
                    ent->is_colliding = true;
                    ent->position[0] = (ent->hit_box_offset[0] / 2) * -1;
                }
                else if(ent->position[0] + ent->hit_box[0] + ent->hit_box_offset[0] + 
                    ent->velocity[0] * global_delta_time * loaded_scene->scene_game_speed >= VIRTUAL_WIDTH)
                {
                    x_colliding = true;
                    ent->is_colliding = true;
                    ent->position[0] = VIRTUAL_WIDTH - (ent->hit_box[0] + ent->hit_box_offset[0]);
                }

                if(glm_aabb2d_aabb(ent_box_y_check, to_collide)) 
                {
                    y_colliding = true;
                    ent->is_colliding = true;

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

    // check collisions with other entities, dont collide with entities on the same team

    for(int i = 0; i < MAX_COLLIDING_ENTITIES; i++)
    {
        ent->colliding_entities[i] = NULL;
    }

    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        entity* other = &loaded_scene->entities[i];

        // skip self
        if(other == ent) 
            continue;

        if(!other->enabled || !other->collision_enabled || other->team == ent->team || other->marked_for_garbage_collection)
        {
            // remove stale symmetric collision entry
            for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
            {
                if(other->colliding_entities[z] == ent)
                    other->colliding_entities[z] = NULL;

                if(ent->colliding_entities[z] == other)
                    ent->colliding_entities[z] = NULL;
            }

            continue;
        }

        if(ent->is_projectile && other->is_projectile)
        {
            // symmetric cleanup
            for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
            {
                if(other->colliding_entities[z] == ent)
                    other->colliding_entities[z] = NULL;

                if(ent->colliding_entities[z] == other)
                    ent->colliding_entities[z] = NULL;
            }

            continue;
        }

        vec2 other_bb[2];
        other_bb[0][0] = other->position[0] + other->hit_box_offset[0] +
                        other->velocity[0] * global_delta_time * loaded_scene->scene_game_speed;
        other_bb[0][1] = other->position[1] + other->hit_box_offset[1] +
                        other->velocity[1] * global_delta_time * loaded_scene->scene_game_speed;

        other_bb[1][0] = other->position[0] + other->hit_box_offset[0] + other->hit_box[0] +
                        other->velocity[0] * global_delta_time * loaded_scene->scene_game_speed;
        other_bb[1][1] = other->position[1] + other->hit_box_offset[1] + other->hit_box[1] +
                        other->velocity[1] * global_delta_time * loaded_scene->scene_game_speed;

        vec2 ent_bb[2] = {
            {
                ent->position[0] + ent->hit_box_offset[0] +
                ent->velocity[0] * global_delta_time * loaded_scene->scene_game_speed,

                ent->position[1] + ent->hit_box_offset[1] +
                ent->velocity[1] * global_delta_time * loaded_scene->scene_game_speed
            },
            {
                ent->position[0] + ent->hit_box_offset[0] + ent->hit_box[0] +
                ent->velocity[0] * global_delta_time * loaded_scene->scene_game_speed,

                ent->position[1] + ent->hit_box_offset[1] + ent->hit_box[1] +
                ent->velocity[1] * global_delta_time * loaded_scene->scene_game_speed
            }
        };

        if(glm_aabb2d_aabb(ent_bb, other_bb))
        {
            int ent_slot = -1;
            int oth_slot = -1;

            bool already_ent = false;
            bool already_other = false;

            // ---- find free slot for ent ----
            for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
            {
                if(ent->colliding_entities[z] == other)
                {
                    already_ent = true;
                    break;
                }
                if(ent->colliding_entities[z] == NULL && ent_slot == -1)
                    ent_slot = z;
            }

            // find free slot for other
            for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
            {
                if(other->colliding_entities[z] == ent)
                {
                    already_other = true;
                    break;
                }
                if(other->colliding_entities[z] == NULL && oth_slot == -1)
                    oth_slot = z;
            }

            // add symmetric collision entries
            if(!already_ent && ent_slot != -1)
                ent->colliding_entities[ent_slot] = other;

            if(!already_other && oth_slot != -1)
                other->colliding_entities[oth_slot] = ent;
        }
        else
        {
            // symmetric removal when NOT colliding
            for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
            {
                if(ent->colliding_entities[z] == other)
                    ent->colliding_entities[z] = NULL;

                if(other->colliding_entities[z] == ent)
                    other->colliding_entities[z] = NULL;
            }
        }
    }

    glm_vec2_copy(ent->position, ent->sprite_data.pos);

}

void entity_override_velocity(entity* ent, vec2 vel)
{
    glm_vec2_copy(vel, ent->velocity);
}


//WIP thisll be weird
/*void damage_entity(entity* ent, entity* attacker)
{
    play_override_animation(&ent->animator_component, ANIM_PLAYER_DAMAGE);
    //ply->entity_timer = PLAYER_INVINCIBILITY_TIME_AFTER_HIT;
    ent->velocity[0] = attacker->position[0] >= ent->position[0] ? -100.0f : 100.0f;
    ent->velocity[1] = -200.0f;
    ent->collision_enabled = false;
    //ply->health_points -= ply->colliding_entities[i]->damage;
    camera_shake(15.0f);
    damage_ui_hp(ply);
}*/