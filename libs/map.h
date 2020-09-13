/* ========================================================================== */
/* File: map.h
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Siddharth Agrawal, Alexander Hirsch, Maria Roodnitsky
 * Component name:  map
 *
 * Date Created:    February 25, 2020
 * Last Updated:    March 9, 2020
 * 
 * Description:     This header file provides the interface with the map module. The map module 
 *                  represents the relationships between cells in the maze.
 *
 *                  The possible states for a relationship between cells are 'unknown', 
 *                  'wall', and 'open'.
 * 
 *                  Status can be get and set through functions like isWall and setWall.
 *                  These functions are parallel in function between the versions for 'unknown',
 *                  'wall', and 'open'. Two versions which accept Cartesian coordinates or 
 *                  position structs are provided for each function, enabling flexibility for
 *                  the caller to use the version which best matches their usage pattern.
 *
 */
/* ========================================================================== */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "AMlib_avatar.h"

/**************** global types ****************/
typedef struct map map_t;

/**************** functions ****************/

// Map create and map delete
map_t *map_new(int mazeWidth, int mazeHeight);
bool map_delete(map_t *map);

// // Check if a position is a valid map location (makes it easy to check if you are trying to enter a cell out of bounds);
// bool map_validPos(map_t *map, position_t *pos);
bool map_validXY(map_t *mp, int x, int y);

// Check if a pair of positions are cardinal-neighbors and are both valid (i.e. on the map)
bool map_neighborAndValidXY(map_t *mp, int x1, int y1, int x2, int y2);

// // Bools to get the status of a relationship between cells:
bool map_isUnknown(map_t *mp, position_t *pos1, position_t *pos2);
bool map_isOpen(map_t *mp, position_t *pos1, position_t *pos2);
bool map_isWall(map_t *mp, position_t *pos1, position_t *pos2);
bool map_isUnknownXY(map_t *mp, position_t *pos1, position_t *pos2);
bool map_isOpenXY(map_t *mp, int x1, int y1, int x2, int y2);
bool map_isWallXY(map_t *mp, int x1, int y1, int x2, int y2);

// // Setters for the status of a relationship between cells
bool map_setUnknown(map_t *mp, position_t *pos1, position_t *pos2); 
bool map_setOpen(map_t *mp, position_t *pos1, position_t *pos2);
bool map_setWall(map_t *mp, position_t *pos1, position_t *pos2);
bool map_setUnknownXY(map_t *mp, int x1, int y1, int x2, int y2);  
bool map_setOpenXY(map_t *mp, int x1, int y1, int x2, int y2);
bool map_setWallXY(map_t *mp, int x1, int y1, int x2, int y2);

// Getters for map and maze info 
int map_getMazeWidth(map_t* mp);





