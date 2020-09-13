/* ========================================================================== */
/* File: simpleprint.h
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Maria Roodnitsky, Andrw Yang, Siddharth Agrawal, Alexander Hirsch
 * Component name:  simpleprint
 *
 * Date Created:    March 3, 2020
 * Last Updated:    March 7, 2020
 * 
 * Description:     This header file exports a function needed for printing an ASCII 
 *                  representation of a maze, represented as a map object and
 *                  avatar_array object
 *             
 */
/* ========================================================================== */

// Import C Standard libraries
#include <stdlib.h>
#include <stdio.h>

// Import project-specific libraries
#include "map.h"
#include "AMlib_avatar.h"

/**************** print_map ****************/
/* print_map prints a graphical ASCII based representation of the maze.
 * And it's supr cool!
 * Inputs:
 *  - HEIGHT:   Height of maze
 *  - WIDTH:    Width of maze
 *  - av_array: Pointer to an array of avatars
 *  - num_av:   Number of avatars
 *  - map:      Pointer to an initialized map structure
 * Memory: does not allocate or delete heap memory passed into it
 */
void print_map(int HEIGHT, int WIDTH, avatar_t **av_array, int num_av, map_t *map);