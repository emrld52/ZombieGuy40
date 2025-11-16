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
        .speed = 512.0f,
        .muzzle_flash_animation = ANIM_BULLET_DEAFULT_FLASH,
        .bullet_zoom_animation = ANIM_BULLET_DEAFULT_ZOOM
    };
}

// WIP

bullet *make_bullet()
{
    return NULL;
}