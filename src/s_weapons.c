#include "s_weapons.h"
#include "s_sound.h"

bullet_type REGULAR_BULLETS;
bullet_type PIERCING_BULLETS;
bullet_type DOUBLE_PIERCING_BULLETS;
bullet_type ENEMY_BULLETS;
bullet_type THROWING_KNIFE;

bullet bullet_object_pool[MAX_BULLETS];

void init_weapon_system()
{
    REGULAR_BULLETS = (bullet_type)
    {
        .damage = 1,
        .gravity = 0,
        .hit_box = {28, 8},
        .hit_box_offset = {2, 12},
        .speed = 1024.0f,
        .muzzle_flash_animation = ANIM_BULLET_DEAFULT_FLASH,
        .bullet_zoom_animation = ANIM_BULLET_DEAFULT_ZOOM,
        .pierce_count = 1,
        .sfx = "regular_shot.mp3"
    };

    PIERCING_BULLETS = (bullet_type)
    {
        .damage = 1,
        .gravity = 0,
        .hit_box = {28, 8},
        .hit_box_offset = {2, 12},
        .speed = 1024.0f,
        .muzzle_flash_animation = ANIM_BULLET_BLUE_FLASH,
        .bullet_zoom_animation = ANIM_BULLET_BLUE_ZOOM,
        .pierce_count = 1,
        .sfx = "piercing_shot.mp3"
    };

    DOUBLE_PIERCING_BULLETS = (bullet_type)
    {
        .damage = 1,
        .gravity = 256.0f,
        .hit_box = {28, 8},
        .hit_box_offset = {2, 12},
        .speed = 1024.0f,
        .muzzle_flash_animation = ANIM_BULLET_GREEN_FLASH,
        .bullet_zoom_animation = ANIM_BULLET_GREEN_ZOOM,
        .pierce_count = 1,
        .sfx = "double_piercing_shot.mp3"
    };

    ENEMY_BULLETS = (bullet_type)
    {
        .damage = 1,
        .gravity = 0,
        .hit_box = {28, 8},
        .hit_box_offset = {2, 12},
        .speed = 512.0f * 0.75f,
        .muzzle_flash_animation = ANIM_BULLET_ENEMY_FLASH,
        .bullet_zoom_animation = ANIM_BULLET_ENEMY_ZOOM,
        .pierce_count = 1,
        .sfx = "enemy_shoot.mp3"
    };

    THROWING_KNIFE = (bullet_type)
    {
        .damage = 1,
        .gravity = 4096.0f,
        .hit_box = {28, 8},
        .hit_box_offset = {2, 12},
        .speed = 512.0f * 0.75f,
        .muzzle_flash_animation = ANIM_BULLET_THROWING_KNIFE,
        .bullet_zoom_animation = ANIM_BULLET_THROWING_KNIFE,
        .pierce_count = 1,
        .sfx = "knife_throw.mp3",
        .is_bouncy = true
    };
}

void bullets_update() {
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullet_object_pool[i].enabled && bullet_object_pool[i].entity != NULL)
        {
            glm_vec2_copy(bullet_object_pool[i].entity->velocity, bullet_object_pool[i].stored_vel);

            bullet_object_pool[i].bounce_time_immunity -= global_delta_time * loaded_scene->scene_game_speed;

            for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
            {
                if(bullet_object_pool[i].entity->colliding_entities[z] != NULL && bullet_object_pool[i].entity->colliding_entities[z]->enabled
                && bullet_object_pool[i].entity->colliding_entities[z]->team != bullet_object_pool[i].entity->team
                && !bullet_object_pool[i].entity->colliding_entities[z]->marked_for_garbage_collection)
                {
                    bool skip_iteration = false;

                    for(int x = 0; x < COLLISION_HISTORY_LIMIT; x++)
                    {
                        if(bullet_object_pool[i].collision_history[x] == NULL)
                        {
                            bullet_object_pool[i].collision_history[x] = bullet_object_pool[i].entity->colliding_entities[z];
                            break;
                        }
                        else if(bullet_object_pool[i].collision_history[x] == bullet_object_pool[i].entity->colliding_entities[z]) skip_iteration = true;
                    }

                    if(!skip_iteration) bullet_object_pool[i].pierces_left -= 1;
                }
            }

            // or if just colliding with level geometry

            if(bullet_object_pool[i].entity->is_colliding && bullet_object_pool[i].bounce_time_immunity <= 0) 
            {
                bullet_object_pool[i].bounces_left -= 1;

                play_override_animation(&bullet_object_pool[i].entity->animator_component, bullet_object_pool[i].type->muzzle_flash_animation);

                if(bullet_object_pool[i].entity->colliding_on_x) {
                    bullet_object_pool[i].entity->velocity[0] *= -1;
                    bullet_object_pool[i].entity->sprite_data.flip_x *= -1;
                }
                
                if(bullet_object_pool[i].entity->colliding_on_y) {
                    if(!bullet_object_pool[i].type->is_bouncy) bullet_object_pool[i].entity->velocity[1] = bullet_object_pool[i].entity->velocity[1] * -1;
                    else bullet_object_pool[i].entity->velocity[1] = -BOUNCE_UP_STRENGTH;
                }

                if(bullet_object_pool[i].bounces_left >= 0) play_sound("projectile_bounce.mp3");

                bullet_object_pool[i].bounce_time_immunity = BOUNCE_TIME_IMMUNITY;
            }
            
            if(bullet_object_pool[i].pierces_left <= 0) 
            {
                bullet_object_pool[i].enabled = false;
                destroy_entity_in_scene(bullet_object_pool[i].entity);
                bullet_object_pool[i].entity = NULL;
            }
            else if(bullet_object_pool[i].bounces_left < 0)
            {
                bullet_object_pool[i].enabled = false;
                destroy_entity_in_scene(bullet_object_pool[i].entity);
                bullet_object_pool[i].entity = NULL;
            }
        }
    }
}

// WIP (maybe)

bullet *make_bullet(bullet_type *typ, vec2 pos, int dir, int team) {
    for(int i = 0; i < MAX_BULLETS; i++) {
        if(!bullet_object_pool[i].enabled) {
            bullet_object_pool[i].entity = make_entity_in_scene(loaded_scene);
            if(bullet_object_pool[i].entity != NULL) {
                // reset bullet collision

                for(int z = 0; z < COLLISION_HISTORY_LIMIT; z++) bullet_object_pool[i].collision_history[z] = NULL;

                play_sound(typ->sfx);
                
                bullet_object_pool[i].entity->is_projectile = true;
                bullet_object_pool[i].enabled = true;
                bullet_object_pool[i].entity->collision_enabled = true;

                bullet_object_pool[i].entity->sprite_data.resolution[0] = 32; bullet_object_pool[i].entity->sprite_data.resolution[1] = 32; 

                // flip sprite if going left
                if(dir == -1) bullet_object_pool[i].entity->sprite_data.flip_x = true;

                bullet_object_pool[i].type = typ;

                bullet_object_pool[i].entity->damage = typ->damage;
                bullet_object_pool[i].entity->velocity[0] = dir == -1 ? -typ->speed : typ->speed;
                bullet_object_pool[i].entity->gravity = typ->gravity;
                bullet_object_pool[i].entity->hit_box[0] = typ->hit_box[0]; bullet_object_pool[i].entity->hit_box[1] = typ->hit_box[1];
                bullet_object_pool[i].entity->hit_box_offset[0] = typ->hit_box_offset[0]; bullet_object_pool[i].entity->hit_box_offset[1] = typ->hit_box_offset[1];

                bullet_object_pool[i].entity->team = team;

                bullet_object_pool[i].pierces_left = typ->pierce_count;
                bullet_object_pool[i].bounces_left = typ->bounce_count;

                // muzzle flash
                play_override_animation(&bullet_object_pool[i].entity->animator_component, typ->muzzle_flash_animation);
                play_animation(&bullet_object_pool[i].entity->animator_component, &typ->bullet_zoom_animation);
                
                bullet_object_pool[i].entity->position[0] = dir > 0 ? pos[0] + typ->hit_box_offset[0] * 4 : pos[0] - typ->hit_box_offset[0] * 4; 
                bullet_object_pool[i].entity->position[1] = pos[1];

                // if knife, throw

                if(bullet_object_pool[i].type == &THROWING_KNIFE) bullet_object_pool[i].entity->velocity[1] = -BOUNCE_UP_STRENGTH; 
                
                return &bullet_object_pool[i];
            }

            bullet_object_pool[i].entity = NULL;

            break;
        }
    }

    return NULL;
}