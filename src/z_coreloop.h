#pragma once

// game logic

void run_gameloop();
void gameloop_init();

// program life cycle stuff

void program_loop();
void program_cleanup();
void program_event();
void program_init();

#define MAX_LOADED_SCENES 4

void damage();

void fix_hp();