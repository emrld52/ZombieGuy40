#pragma once

#include "s_entities.h"
#include "r_renderfuncs.h"
#include "s_animation.h"

void init_hp_ui(entity *plyr);
void damage_ui_hp(entity *plyr);
void heal_ui_hp(entity *plyr);
void draw_hp_ui();
void draw_player_stats(int dmg, int attack_speed, bool is_auto, int pierces, int bounces, int total_killed);
void draw_cursor();