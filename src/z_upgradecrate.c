#include "z_upgradecrate.h"
#include "s_entities.h"
#include "s_scene.h"
#include "g_state.h"

entity *crate;
sprite crate_sprite;

float time_til_despawn;

bool has_played_flash = false;

int zombies_killed_total = 0;
int zombies_killed = 0;

void init_supply_crate()
{
    crate_sprite = (sprite)
    {
        .sprite_coord[0] = 14,
        .sprite_coord[1] = 17,
        .resolution[0] = 32,
        .resolution[1] = 64,
    };

    crate = make_entity_in_scene(loaded_scene);
    crate->gravity = CRATE_GRAVITY;
    crate->is_projectile = true;
    crate->team = 0;
    crate->sprite_data = crate_sprite;

    crate->id = CRATE_ID;

    crate->collision_enabled = true;

    crate->hit_box[0] = 32; crate->hit_box[1] = 32;
    crate->hit_box_offset[1] = 32;

    crate->position[0] = rand() % VIRTUAL_WIDTH;
    crate->position[1] = -64;

    crate->velocity[0] = rand() % 2 == 1 ? CRATE_VELOCITY : -CRATE_VELOCITY;

    animator_init(&crate->animator_component);
    play_animation(&crate->animator_component, &ANIM_CRATE_FALLING);

    time_til_despawn = CRATE_LIFETIME;

    has_played_flash = false;
}

void destroy_crate()
{
    destroy_entity_in_scene(crate);
    crate = NULL;
}

void update_supply_crate()
{
    if(crate != NULL && crate->is_grounded) 
    {
        play_animation(&crate->animator_component, &ANIM_CRATE_LANDED);
        crate->handle_x_for_me = true;
        time_til_despawn -= global_delta_time * loaded_scene->scene_game_speed;

        if(time_til_despawn <= 0) destroy_crate();
        else if(time_til_despawn <= 1.0f && !has_played_flash)
        {
            has_played_flash = true;
            play_override_animation(&crate->animator_component, ANIM_CRATE_FLASH);
        }
    }
    else if(crate == NULL)
    {
        if(zombies_killed >= REQUIREMENT_FOR_CRATE) 
        {
            init_supply_crate();

            // if player killed more whilst crate was still around

            zombies_killed -= REQUIREMENT_FOR_CRATE;
            if(zombies_killed < 0) zombies_killed = 0;
        }
    }
}