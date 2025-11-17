#pragma once

#define CRATE_GRAVITY 128.0f
#define CRATE_VELOCITY 32.0f
#define CRATE_ID 1000
#define CRATE_LIFETIME 7.5f

#define MIN_TIME_TIL_SPAWN 10
#define MAx_TIME_TIL_SPAWN 20

void init_supply_crate();
void update_supply_crate();
void destroy_crate();