/* ========================================================================== */
/* File: simpleprint.c
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Maria Roodnitsky, Andrw Yang, Siddharth Agrawal, Alexander Hirsch
 * Component name:  simpleprint
 *
 * Date Created:    March 3, 2020
 * Last Updated:    March 7, 2020
 * 
 * Description:     This file supports functions needed for printing an ASCII 
 *                  representation of a maze, represented as a map object and
 *                  avatar_array object
 *             
 */
/* ========================================================================== */

// Include Standard C libraries
#include <stdlib.h>
#include <stdio.h>

// Include project-specific libraries
#include "map.h"
#include "AMlib_avatar.h"


/*** Print function referenced outside file ***/
void print_map(int HEIGHT, int WIDTH, avatar_t **av_array, int num_av, map_t *map);

/*** Helper function prototype ***/
static void print_column_header(int WIDTH);
static void print_map_border(int WIDTH);

/*** Functions ***/

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
void print_map(int HEIGHT, int WIDTH, avatar_t **av_array, int num_av, map_t *map)
{
  
  // Print the column header and the top map border
  print_column_header(WIDTH);
  print_map_border(WIDTH);
  
  // For each row ...
  for (int j = 0; j < HEIGHT; j++) {

    // Print the first side-wall along with the row number
    printf("%3d|", j);

    // For each cell in the row
    for (int i = 0; i < WIDTH; i++){

      // Check for the presence of an avatar
      int avatarID;

      // If found, print the lowest numbered-avatar into the cell
      if((avatarID = avatar_array_find_avatar(av_array, num_av, i, j)) >= 0){
        printf(" %d ", avatarID); 
      } else {
        // If no avatar is present, print blank space
        printf("   ");
      }

      // Check internal borders for existence of vertical walls
      // If found, print the wall representation; if not, print blank 
      if ((i < WIDTH - 1) && map_isWallXY(map, i, j, i + 1, j)){
        printf("|"); 
      } else {
        printf(" ");
      }
    }

    // Handle special case of the end of the row: backspace-by-one and print wall
    printf("\b|\n");  
    
    // Print the bottom line of each row (spaces for open paths, and dashes for discovered walls)
    if (j != HEIGHT  - 1){ 

      // Tab-over the beginning of the line
      printf("   +");

      // Print the appropriate wall or space
      for (int i = 0; i < WIDTH; i++){
        if (map_isWallXY(map, i, j, i, j + 1)){
          printf("---"); 
        } else{
          printf("   ");
        }       
        printf("+");    
      }
    
    // Go to new line
    printf("\n"); 
    }
  }
  
  // Print the bottom border of the map
  print_map_border(WIDTH);
}

/**************** static print_column_header ****************/
//line that prints "   0   1   2   3 ..." at the top of the table
static void print_column_header(int WIDTH){
    printf("   ");
  
  for (int i = 0; i < WIDTH; i++){
    printf("%3d ", i);
  }
  printf("\n");
}

/**************** static print_map_border ****************/
//line that prints "+---+---+---..." on the top and bottom of the map
static void print_map_border(int WIDTH){
    printf("   ");
  for (int i = 0; i < WIDTH; i++){
    printf("+---");
  }
  printf("+\n");
}