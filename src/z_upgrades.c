#include "g_state.h"
#include "r_renderfuncs.h"
#include "s_entities.h"
#include "z_upgrades.h"
#include "z_player.h"
#include "s_menu.h"
#include "s_text_renderer.h"
#include "z_player.h"
#include "s_weapons.h"
#include "s_playerUI.h"

#include <string.h>

upgrade upgrades[UPGRADE_COUNT];

bool is_upgrade_menu_open = false;

int rerolls = DEAFULT_REROLLS;
int selected_upgrade;

void init_upgrades()
{
    upgrades[UPGRD_ASSAULT_RIFLE] = (upgrade)
    {
        .name = "assault rifle",
        .desc = "add 20p fire rate/nauto fire/nreduce bullet drop",
        .does_stack = true,
        .sprite_coord[0] = 9,
        .sprite_coord[1] = 9,
    };

    upgrades[UPGRD_SNIPER_RIFLE] = (upgrade)
    {
        .name = "sniper rifle",
        .desc = "add 1 damage/nadd 2 pierce/nsingle fire/nadd bullet drop",
        .does_stack = true,
        .sprite_coord[0] = 11,
        .sprite_coord[1] = 9,
    };

    upgrades[UPGRD_PLASMA_RIFLE] = (upgrade)
    {
        .name = "plasma rifle",
        .desc = "add 30p fire rate/nno bullet drop",
        .does_stack = true,
        .sprite_coord[0] = 13,
        .sprite_coord[1] = 9,
    };

    upgrades[UPGRD_THROWING_KNIFE] = (upgrade)
    {
        .name = "throwing knives",
        .desc = "add 1 bounce/nadd 2 pierce/nthrowing/nsingle fire/nhuge falloff",
        .does_stack = true,
        .sprite_coord[0] = 9,
        .sprite_coord[1] = 11,
    };

    upgrades[UPGRD_DUAL_WIELD] = (upgrade)
    {
        .name = "dual wielding",
        .desc = "fire in both/ndirections/nadd 10p fire rate",
        .does_stack = false,
        .sprite_coord[0] = 11,
        .sprite_coord[1] = 11,
    };

    upgrades[UPGRD_RUBBER_BULLETS] = (upgrade)
    {
        .name = "rubber bullets",
        .desc = "add 1 bounce/nremove 1 damage",
        .does_stack = true,
        .sprite_coord[0] = 13,
        .sprite_coord[1] = 11,
    };

    upgrades[UPGRD_GAMBLING_CHIP] = (upgrade)
    {
        .name = "gambling chip",
        .desc = "1 extra reroll/nper crate/nremove 1 max hp",
        .does_stack = true,
        .sprite_coord[0] = 9,
        .sprite_coord[1] = 13,
    };

    upgrades[UPGRD_FLIGHT] = (upgrade)
    {
        .name = "flight",
        .desc = "half gravity/nremove 1 max hp/nhold s to go down",
        .does_stack = false,
        .sprite_coord[0] = 11,
        .sprite_coord[1] = 13,
    };

    upgrades[UPGRD_KEVLAR_VEST] = (upgrade)
    {
        .name = "kevlar vest",
        .desc = "add 1 max hp",
        .does_stack = true,
        .sprite_coord[0] = 13,
        .sprite_coord[1] = 13,
    };

    upgrades[UPGRD_DODGE] = (upgrade)
    {
        .name = "dodge",
        .desc = "add 50p/ninvincibility frames",
        .does_stack = true,
        .sprite_coord[0] = 9,
        .sprite_coord[1] = 15,
    };

    upgrades[UPGRD_KAMIKAZE] = (upgrade)
    {
        .name = "kamikaze",
        .desc = "add 1 touch damage",
        .does_stack = true,
        .sprite_coord[0] = 11,
        .sprite_coord[1] = 15,
    };

    /*upgrades[UPGRD_IN_NEED] = (upgrade)
    {
        .name = "in need",
        .desc = "1 lower kill/nrequirement for/nupgrade crate",
        .does_stack = true,
        .sprite_coord[0] = 13,
        .sprite_coord[1] = 15,
    };*/
}

void prepare_upgrade() {
    rerolls = player_upgrade_rerolls;
    is_upgrade_menu_open = true;
    selected_upgrade = rand() % UPGRADE_COUNT;
    force_paused = true;
}

void apply_upgrade_to_player(player *ply, int UPGRADE_ID)
{
    switch(UPGRADE_ID)
    {
        case UPGRD_ASSAULT_RIFLE:
            ply->wpn.fire_rate /= 1.2f;
            ply->wpn.is_auto = true;
            ply->player_bullet_type = &REGULAR_BULLETS;
            break;
        case UPGRD_SNIPER_RIFLE:
            ply->bullet_overrides.damage += 1;
            ply->bullet_overrides.pierces += 2;
            ply->wpn.is_auto = false;
            ply->player_bullet_type = &DOUBLE_PIERCING_BULLETS;
            break;
        case UPGRD_PLASMA_RIFLE:
            ply->wpn.fire_rate /= 1.3f;
            ply->player_bullet_type = &PIERCING_BULLETS;
            break;
        case UPGRD_THROWING_KNIFE:
            ply->bullet_overrides.bounces += 1;
            ply->bullet_overrides.pierces += 2;
            ply->player_bullet_type = &THROWING_KNIFE;
            break;
        case UPGRD_DUAL_WIELD:
            ply->fire_both_directions = true;
            ply->wpn.fire_rate /= 1.1f;
            break;
        case UPGRD_RUBBER_BULLETS:
            ply->bullet_overrides.bounces += 1;
            ply->bullet_overrides.damage -= 1;
            break;
        case UPGRD_GAMBLING_CHIP:
            player_upgrade_rerolls += 1;
            ply->plyr->max_health_points -= 1;
            ply->plyr->health_points -= 1;
            break;
        case UPGRD_FLIGHT:
            ply->plyr->gravity /= 2;
            ply->plyr->max_health_points -= 1;
            ply->plyr->health_points -= 1;
            break;
        case UPGRD_KEVLAR_VEST:
            ply->plyr->max_health_points += 1;
            ply->plyr->health_points += 1;
            break;
        case UPGRD_DODGE:
            ply->invinc_time *= 1.5f;
            break;
        case UPGRD_KAMIKAZE:
            ply->plyr->damage += 1;
            break;
        /*case UPGRD_IN_NEED:
            ply->plyr->damage += 1;
            break;*/
    }

    // dont deal 0 damage and dont allow 0 max hp

    if(ply->bullet_overrides.damage < 0) ply->bullet_overrides.damage = 1;

    if(ply->plyr->max_health_points <= 0)
    {
        ply->plyr->health_points = 1;
        ply->plyr->max_health_points = 1;
    }

    init_hp_ui(ply->plyr);
    heal_ui_hp(ply->plyr);
}

void render_upgrade_menu() {
    render_text(upgrades[selected_upgrade].name, 32, (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(strlen(upgrades[selected_upgrade].name)) / 2), 128});

    draw_call((sprite)
    {
        .resolution[0] = 64, .resolution[1] = 64, 
        .pos[0] = (VIRTUAL_WIDTH / 2) - 32, .pos[1] = UI_DISPLAY_Y_POS + UPGRADE_ICON_RES, 
        .sprite_coord[0] = upgrades[selected_upgrade].sprite_coord[0],
        .sprite_coord[1] = upgrades[selected_upgrade].sprite_coord[1],
        .ui = true,
    });

    render_text(upgrades[selected_upgrade].desc, MAX_UPGRADE_DESC_LENGTH, (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2) - (UPGRADE_ICON_RES / 2), 160 + UPGRADE_ICON_RES * 1.5f});

    if(is_button_clicked(&upgrade_btns[0], (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 })) {
        if(rerolls > 0) {
            rerolls -= 1;
            selected_upgrade = rand() % UPGRADE_COUNT;  
        }
    }
    else if(is_button_clicked(&upgrade_btns[1], (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 + 24 })) {
        apply_upgrade_to_player(&ply, selected_upgrade);
        force_paused = false;
        is_upgrade_menu_open = false;
    }
    else if(is_button_clicked(&upgrade_btns[2], (vec2){ (VIRTUAL_WIDTH / 2) - (how_wide_is_text(10) / 2), 350 + 48})) {
        force_paused = false;
        is_upgrade_menu_open = false;
    }
}