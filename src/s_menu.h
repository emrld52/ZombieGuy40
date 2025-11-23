#pragma once

#include "r_renderfuncs.h"
#include "s_animation.h"

void init_menus();
void draw_main_menu();
void draw_pause_menu();

#define MAX_BUTTON_TEXT_LEN 10

typedef struct button_t
{
    animator button_animation;
    sprite button_strip;
    char txt[MAX_BUTTON_TEXT_LEN];
    bool was_hovered;
} button;
bool is_button_clicked(button *btn, vec2 pos);