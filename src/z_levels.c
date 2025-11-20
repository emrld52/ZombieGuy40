#include "z_levels.h"
#include "s_tilemap.h"

// 3d array acting as list of 2d arrays basically

tile level_geometry_options[LEVEL_GEOMETRY_OPTION_COUNT][LEVELS_HEIGHT][LEVELS_WIDTH];

// pretty unclean solution to the problem and not super flexible but this games levels arent complex

// level geom 1 is the 2 platforms with the 2 mounds on either end of the map
// level geom 2 is the flat ground
// level geom 3 is the singular mound with one platform

void init_level_geometry()
{
    // GEOM 1




    for(int y = 0; y < LEVELS_HEIGHT; y++)
    {
        for(int x = 0; x < LEVELS_WIDTH; x++)
        {
            if(y < 14) level_geometry_options[0][y][x].is_filled = false;
            else level_geometry_options[0][y][x].is_filled = true;
        }
    }

    level_geometry_options[0][10][6].is_filled = true;
    level_geometry_options[0][10][7].is_filled = true;
    level_geometry_options[0][10][8].is_filled = true;
    level_geometry_options[0][9][8].direction_for_pathing = 2;
    level_geometry_options[0][9][10].direction_for_pathing = 2;

    for(int y = 12; y < 16; y++)
    {
        for(int x = 0; x < 4; x++)
        {
            level_geometry_options[0][y][x].is_filled = true;
        }
    }

    level_geometry_options[0][10][19 - 6].is_filled = true;
    level_geometry_options[0][10][19 - 7].is_filled = true;
    level_geometry_options[0][10][19 - 8].is_filled = true;

    for(int y = 12; y < 16; y++)
    {
        for(int x = 16; x < 20; x++)
        {
            level_geometry_options[0][y][x].is_filled = true;
        }
    }

    level_geometry_options[0][13][3].direction_for_pathing = 2;
    level_geometry_options[0][11][2].direction_for_pathing = 3;
    level_geometry_options[0][11][3].direction_for_pathing = 1;
    level_geometry_options[0][13][15].direction_for_pathing = 2;
    level_geometry_options[0][11][16].direction_for_pathing = 4;
    level_geometry_options[0][11][15].direction_for_pathing = 1;
}

void load_level_into_2d_array(int geometry, tile arr[LEVELS_HEIGHT][LEVELS_WIDTH])
{
    for(int y = 0; y < LEVELS_HEIGHT; y++)
    {
        for(int x = 0; x < LEVELS_WIDTH; x++)
        {
            // reset tile
            arr[y][x] = (tile)
            {
                .is_filled = false,
                .direction_for_pathing = 0
            };

            arr[y][x] = level_geometry_options[geometry][y][x];
        }
    }
}