/* ========================================================================== */
/* File: printtest.c
 * *** Category: Testing Only ***
 * *** Not part of compilation path for user-facing executable
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Maria Roodnitsky, Andrw Yang, Siddharth Agrawal, Alexander Hirsch, 
 * Component name:  printtest.c
 *
 * Date Created:    March 3rd, 2020
 *
 * This file is a test driver for the print and map modules.
 * 
 * Compilation:     mygcc printtest.c ../libs/simpleprint.c ../libs/map.c ../libs/AMlib_avatar.c -o printtest
 * Usage:           ./printtest           
 *
 */
/* ========================================================================== */

// Import C Standard libraries
#include <stdio.h>
#include <stdbool.h>

// Import project-specific libraries
#include "../libs/simpleprint.h"
#include "../libs/map.h"
#include "../libs/AMlib_avatar.h"

// Runs a series of tests to validate performance of the simpleprint module
int main(const int argc, const char *argv[])
{
    // Set maze dimensions and create map
    int mazeWidth = 10;
    int mazeHeight = 8;
    map_t *mp = map_new(mazeWidth, mazeHeight);

    // Assign some test positions
    position_t* pos1 = position_new();
    position_setX(pos1, 0);
    position_setY(pos1, 1);

    position_t* pos2 = position_new();
    position_setX(pos2, 1);
    position_setY(pos2, 1);

    position_t* pos3 = position_new();
    position_setX(pos3, 5);
    position_setY(pos3, 1);

    position_t* pos4 = position_new();
    position_setX(pos4, 4);
    position_setY(pos4, 1);
    
    position_t* pos5 = position_new();
    position_setX(pos5, 5);
    position_setY(pos5, 6);
    
    position_t* pos6 = position_new();
    position_setX(pos6, 5);
    position_setY(pos6, 5);
    
    position_t* pos7 = position_new();
    position_setX(pos7, 3);
    position_setY(pos7, 4);

    // Set walls
    map_setWall(mp, pos1, pos2);
    map_setWall(mp, pos5, pos6);

    // Create avatars and set their positions
    int num_avatar = 3;
    avatar_t *av0 = avatar_new(0);
    avatar_setPosition(av0, pos3);
    avatar_t *av1 = avatar_new(1);
    avatar_setPosition(av1, pos4);
    avatar_t *av2 = avatar_new(2);
    avatar_setPosition(av2, pos7);

    // Join avatars into an avatar_array
    avatar_t **avatar_array = avatar_array_new(num_avatar);
    avatar_array_add(avatar_array, av0);
    avatar_array_add(avatar_array, av1);
    avatar_array_add(avatar_array, av2);
    
    // Print time: verify by inspection that all objects allocated
    // above are printed correctly
    print_map(mazeHeight, mazeWidth, avatar_array, num_avatar, mp);

    // Free everything
    map_delete(mp);
    avatar_array_delete(avatar_array, num_avatar);
    free(pos1);
    free(pos2);
    free(pos3);
    free(pos4);
    free(pos5);
    free(pos6);
    free(pos7);

    // Verify completion of execution
    printf("We made it here!\n");

    return 0;
}
