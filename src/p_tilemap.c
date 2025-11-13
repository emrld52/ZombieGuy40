// libs

#include <stdbool.h>
#include "../deps/cglm/cglm.h"

// source

#include "p_tilemap.h"

// each tile name represents a coordinate on our sprite atlas

const vec2 TOP_LEFT_TILE = { 1, 15 };
const vec2 TOP_MID_TILE = { 2, 15 };
const vec2 TOP_RIGHT_TILE = { 3, 15 };
const vec2 TOP_TILE = { 4, 15 };
const vec2 MID_LEFT_TILE = { 1, 16 };
const vec2 MID_MID_TILE = { 2, 16 };
const vec2 MID_RIGHT_TILE = { 3, 16 };
const vec2 MID_TILE = { 4, 16 };
const vec2 BOTTOM_LEFT_TILE = { 1, 17 };
const vec2 BOTTOM_MID_TILE = { 2, 17 };
const vec2 BOTTOM_RIGHT_TILE = { 3, 17 };
const vec2 BOTTOM_TILE = { 4, 17 };
const vec2 HOZ_LEFT_TILE = { 1, 18 };
const vec2 HOZ_MID_TILE = { 2, 18 };
const vec2 HOZ_RIGHT_TILE = { 3, 18 };
const vec2 TILE = { 4, 18 };

void init_tilemap(tile arr[LEVELS_HEIGHT][LEVELS_WIDTH])
{
    for(int y = 0; y < LEVELS_HEIGHT; y++)
    {
        for(int x = 0; x < LEVELS_WIDTH; x++)
        {
            glm_vec2_copy((vec2){ LEVELS_TILE_RESOLUTION, LEVELS_TILE_RESOLUTION }, arr[y][x].tile_sprite.resolution);

            // offset each tile sprite based on its resolution * coordinate so theyre spaced out properly

            glm_vec2_copy((vec2){ LEVELS_TILE_RESOLUTION * x, LEVELS_TILE_RESOLUTION * y}, arr[y][x].tile_sprite.pos);
            arr[y][x].tile_sprite.ui = false;
        }
    }
}

// use of bitwise operations to create a value representing a sprite

void autotiler_build_tilemap(tile arr[LEVELS_HEIGHT][LEVELS_WIDTH])
{
    for(int y = 0; y < LEVELS_HEIGHT; y++)
    {
        for(int x = 0; x < LEVELS_WIDTH; x++)
        {
            if(arr[y][x].is_filled) 
            {
                // keep track of what the surrounding tiles are through bitmask

                int tile_value = 0;

                if(y > 0 && arr[y - 1][x].is_filled) tile_value |= (1u << 0);
                if(x < LEVELS_WIDTH - 1 && arr[y][x + 1].is_filled) tile_value |= (1u << 1);
                if(y < LEVELS_HEIGHT - 1 && arr[y + 1][x].is_filled) tile_value |= (1u << 2);
                if(x > 0 && arr[y][x - 1].is_filled) tile_value |= (1u << 3);

                // apply sprite coordinate based on bitmask

                switch(tile_value)
                {
                case 0:
                    glm_vec2_copy(TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 1:
                    glm_vec2_copy(BOTTOM_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 2:
                    glm_vec2_copy(HOZ_LEFT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 3:
                    glm_vec2_copy(BOTTOM_LEFT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 4:
                    glm_vec2_copy(TOP_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 5:
                    glm_vec2_copy(MID_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 6:
                    glm_vec2_copy(TOP_LEFT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 7:
                    glm_vec2_copy(MID_LEFT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 8:
                    glm_vec2_copy(HOZ_RIGHT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 9:
                    glm_vec2_copy(BOTTOM_RIGHT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 10:
                    glm_vec2_copy(HOZ_MID_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 11:
                    glm_vec2_copy(BOTTOM_MID_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 12:
                    glm_vec2_copy(TOP_RIGHT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 13:
                    glm_vec2_copy(MID_RIGHT_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 14:
                    glm_vec2_copy(TOP_MID_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                case 15:
                    glm_vec2_copy(MID_MID_TILE, arr[y][x].tile_sprite.sprite_coord);
                    break;
                }
            }
        }
    }
}