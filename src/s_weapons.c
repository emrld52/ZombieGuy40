#include "s_weapons.h"

bullet_type REGULAR_BULLETS;

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
        .bullet_zoom_animation = ANIM_BULLET_DEAFULT_ZOOM
    };
}

void bullets_update()
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullet_object_pool[i].enabled && &bullet_object_pool[i].entity != NULL)
        {
            bool destroy_ent = false;

            for(int z = 0; z < MAX_COLLIDING_ENTITIES; z++)
            {
                if(bullet_object_pool[i].entity->colliding_entities[z] != NULL 
                    && bullet_object_pool[i].entity->colliding_entities[z]->collision_enabled)
                {
                    bullet_object_pool[i].enabled = false;
                    destroy_entity_in_scene(bullet_object_pool[i].entity);
                    destroy_ent = true;
                }
            }

            // or if just colliding with level geometry

            if(bullet_object_pool[i].entity->is_colliding)
            {
                bullet_object_pool[i].enabled = false;
                destroy_entity_in_scene(bullet_object_pool[i].entity);
                destroy_ent = true;
            }
            
            if(destroy_ent) bullet_object_pool[i].entity = NULL;
        }
    }
}

// WIP

bullet *make_bullet(bullet_type *typ, vec2 pos, int dir, int team)
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(!bullet_object_pool[i].enabled)
        {
            bullet_object_pool[i].entity = make_entity_in_scene(loaded_scene);
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

            // muzzle flash
            play_override_animation(&bullet_object_pool[i].entity->animator_component, typ->muzzle_flash_animation);
            play_animation(&bullet_object_pool[i].entity->animator_component, &typ->bullet_zoom_animation);
            
            bullet_object_pool[i].entity->position[0] = dir > 0 ? pos[0] + typ->hit_box_offset[0] * 4 : pos[0] - typ->hit_box_offset[0] * 4; 
            bullet_object_pool[i].entity->position[1] = pos[1];
            
            return &bullet_object_pool[i];
        }
    }

    return NULL;
}