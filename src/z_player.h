#pragma once

#include "r_renderfuncs.h"
#include "s_entities.h"

#define PLAYER_INVINCIBILITY_TIME_AFTER_HIT 0.65f

void player_init();
void player_loop();
entity* get_player();