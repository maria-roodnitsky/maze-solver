/* ========================================================================== */
/* File: maptest.c
 * *** Category: Testing Only ***
 * *** Not part of compilation path for user-facing executable
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Maria Roodnitsky, Andrw Yang, Siddharth Agrawal, Alexander Hirsch, 
 * Component name:  maptest.c
 *
 * Date Created:    March 3rd, 2020
 *
 * This file is a test driver for the map module.
 * 
 * Compilation:     mygcc maptest.c ../libs/map.c ../libs/AMlib_avatar.c -o maptest
 * Usage:           ./maptest           
 *
 */
/* ========================================================================== */

// Include C Standard libraries
#include <stdio.h>
#include <stdbool.h>

// Include project-specific libraries
#include "../libs/map.h"
#include "../libs/AMlib_avatar.h"

// Runs unit testing for the map module
int main(const int argc, const char *argv[])
{
    // Set map parameters and initialize map
    int mazeWidth = 4;
    int mazeHeight = 5;
    map_t *mp = map_new(mazeWidth, mazeHeight);

    // Create some positions inside the map
    position_t* pos1 = position_new();
    position_setX(pos1, 1);
    position_setY(pos1, 2);

    position_t* pos2 = position_new();
    position_setX(pos2, 1);
    position_setY(pos2, 1);

    position_t* pos3 = position_new();
    position_setX(pos3, 1);
    position_setY(pos3, 0);

    // Test 'wall' get and set functions
    map_setWall(mp, pos1, pos2);
    if (map_isWall(mp, pos1, pos2)) {
        printf("The wall is a wall!\n");
    } else {
        printf("The wall should have been a wall... but it isn't?\n");
    }

    if (!map_isWall(mp, pos2, pos3)) {
        printf("The not-a-wall isn't a wall!\n");
    } else {
        printf("This shouldn't be a wall...but it is\n");
    }

    // Test 'open' get and set functions
    map_setOpenXY(mp, 2, 2, 2, 3);
    if (map_isOpenXY(mp, 2, 2, 2, 3)) {
        printf("It's open and should be!\n");
    } else {
        printf("It's not open but it should be\n");
    }
    if (!map_isWallXY(mp, 2, 2, 2, 3)) {
        printf("The not-a-wall isn't a wall!\n");
    } else {
        printf("This shouldn't be a wall...but it is\n");
    }

    // Test XY version of functions
    map_setWallXY(mp, 2, 2, 2, 1);
    if (map_isWallXY(mp, 2, 2, 2, 1)) {
        printf("The wall is a wall!\n");
    } else {
        printf("The wall should have been a wall... but it isn't?\n");
    }

    // Test neighborAndValid functions
    if(map_neighborAndValidXY(mp, 2, 2, 2, 1)) {
        printf("Correct outcome.\n");
    } else {
        printf("Incorrect outcome.\n");
    }
    if(!map_neighborAndValidXY(mp, 2, 2, 1, 1)) {
        printf("Correct outcome.\n");
    } else {
        printf("Incorrect outcome.\n");
    }
    if(!map_neighborAndValidXY(mp, -1, 0, 0, 0)) {
        printf("Correct outcome.\n");
    } else {
        printf("Incorrect outcome.\n");
    }

    // Test behavior on invalid positions
    if (!map_setWallXY(mp, -1, 0, 0, 0)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }
    if (!map_setWallXY(mp, 2, 2, 1, 1)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }
    if (map_isWallXY(mp, -1, 0, 0, 0)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }
    if (!map_isWallXY(mp, 2, 2, 1, 1)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }
    if (map_isWallXY(mp, -1, 0, 0, 0)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }
    if (map_isWallXY(mp, 2, -1, 2, 0)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }
    if (map_isWallXY(mp, 4, 0, 3, 0)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }
    if (map_isWallXY(mp, 3, 0, 4, 0)) {
        printf("Correct outcome\n");
    } else {
        printf("Incorrect outcome\n");
    }

    // Clean up
    position_delete(pos1);
    position_delete(pos2);
    position_delete(pos3);
    map_delete(mp);
    
    return 0;
}