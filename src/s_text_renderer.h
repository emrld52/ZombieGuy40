#include "r_renderfuncs.h"

#define LETTER_COUNT 26

#define FONT_SHEET_WIDTH 9
#define FONT_SHEET_HEIGHT 4
#define FONT_SHEET_X 12
#define FONT_SHEET_Y 5

#define LETTER_DISTANCE 15

// sprites for each letter

extern sprite LETTER_SPRITES[LETTER_COUNT];
extern sprite NUMBER_SPRITES[10];

void init_text_renderer();
void render_text(char txt[], int len, vec2 pos);