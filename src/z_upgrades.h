#pragma once

#include "r_renderfuncs.h"
#include "s_entities.h"
#include "z_player.h"

// for vectors for sprite coords

#include "../deps/cglm/cglm.h"

#define MAX_UPGRADE_NAME_LENGTH 32
#define MAX_UPGRADE_DESC_LENGTH 128

#define UPGRADE_COUNT 11

#define DEAFULT_REROLLS 2

#define UPGRADE_ICON_RES 64

#define UI_DISPLAY_Y_POS 96

typedef struct upgrade_t
{
    char name[MAX_UPGRADE_NAME_LENGTH];
    char desc[MAX_UPGRADE_DESC_LENGTH];
    vec2 sprite_coord;
    bool does_stack;
} upgrade;

extern upgrade upgrades[UPGRADE_COUNT];

typedef enum UPGRADE_IDs {
    UPGRD_ASSAULT_RIFLE = 0,
    UPGRD_SNIPER_RIFLE = 1,
    UPGRD_PLASMA_RIFLE = 2,
    UPGRD_THROWING_KNIFE = 3,
    UPGRD_DUAL_WIELD = 4,
    UPGRD_RUBBER_BULLETS = 5,
    UPGRD_GAMBLING_CHIP = 6,
    UPGRD_FLIGHT = 7,
    UPGRD_KEVLAR_VEST = 8,
    UPGRD_DODGE = 9,
    UPGRD_KAMIKAZE = 10,
    UPGRD_IN_NEED = 11,
};

extern bool is_upgrade_menu_open;

void init_upgrades();
void prepare_upgrade();
void render_upgrade_menu();
void apply_upgrade_to_player(player *ply, int UPGRADE_ID);