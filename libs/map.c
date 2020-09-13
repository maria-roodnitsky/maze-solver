/* ========================================================================== */
/* File: map.c
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Siddharth Agrawal, Alexander Hirsch, Maria Roodnitsky
 * Component name:  map.c
 *
 * Date Created:    February 25, 2020
 * Date Updated:    March 8, 2020
 * 
 *
 * Description:     This file implements a map of the maze. It exports several structs 
 *                  and functions for use by other components. It implements its own 
 *                  data structure to store the map.
 * 
 *                  The map is implemented as a 2D data grid, where the dimensions are:
 *                      dataHeight = 2*MazeHeight - 1
 *                      dataWidth = 2*MazeWidth - 1
 *                  
 *                  In this data structure, even indexed rows and column pairs e.g. (0,0),
 *                  (0,2), (2,0) represent cells, while odd numbered rows and columns e.g.
 *                  (0,1), (1,0) represent relationships between cells
 *                  
 *                  The possible states for a relationship between cells are 'unknown', 
 *                  'wall', and 'open'. State of cells is represented with integer values 
 *                  (e.g. wallNum, openNum, unknownNum).
 * 
 *                  Status can be get and set through functions like isWall and setWall.
 *                  These functions are parallel in function between the versions for 'unknown',
 *                  'wall', and 'open'. Two versions which accept Cartesian coordinates or 
 *                  position structs are provided for each function, enabling flexibility for
 *                  the caller to use the version which best matches their usage pattern.
 *                  
 *                  A supporting findDataLocation function converts users input points into
 *                  locations in the data structure, and checks for validity of input
 *
 */
/* ========================================================================== */

// Import C Standard libraries
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Import project specific libraries
#include "AMlib_avatar.h"


/**************** global types ****************/
typedef struct map {
    int **columns;      // Stores pointers to each column
    int mazeHeight;
    int mazeWidth;
    int dataHeight;
    int dataWidth;
    int unknownNum;     // Representative value for 'unknown'
    int openNum;        // Representative value for 'unknown'
    int wallNum;        // Representative value for 'unknown'
} map_t;


/**************** Function prototypes ****************/
static position_t *findDataLocation(map_t *mp, position_t *pos1, position_t *pos2);
bool map_validXY(map_t *mp, int x, int y);
bool map_neighborAndValidXY(map_t *mp, int x1, int y1, int x2, int y2);


/**************** map_new ****************/
/* Memory: returns a newly allocated map object. Caller must later free this memory
 * using map_delete.
 * Implementation: The grid is implemented as an array of arrays. The senior array
 * represents row position, and the junior array corresponding to each row position
 * is a column. As a result, calls can be performed against the map of the format [x][y]. 
 * 
 */
map_t* map_new(int mazeWidth, int mazeHeight) 
{

    // Allocate and initialize the map struct
    map_t *mp = malloc(sizeof(map_t));
    mp->mazeWidth = mazeWidth;
    mp->mazeHeight = mazeHeight;
    mp->dataWidth = 2*mazeWidth - 1;
    mp->dataHeight = 2*mazeHeight - 1;
    mp->unknownNum = 0;
    mp->openNum = 1;
    mp->wallNum = 2;
    mp->columns = (int **)malloc(mp->dataWidth * sizeof(int *));

    // Allocate all the columns
    for (int i = 0; i < mp->dataWidth; i++) {
        mp->columns[i] = (int *)malloc(mp->dataHeight * sizeof(int));
    }

    // Initialize each map location to the value of unknown
    for (int i = 0; i < (2*mazeWidth - 1); i++) {
        for (int j = 0; j < (2*mazeHeight - 1); j++) {
            mp->columns[i][j] = mp->unknownNum;

        }
    }

    return mp;
}

/**************** map_delete ****************/
/* Memory: Frees all memory associated with the map data structure
 * 
 */
bool map_delete(map_t *mp) 
{
    // Free each of the columns
    for (int i = 0; i < mp->dataWidth; i++) {
        free(mp->columns[i]);
    }

    // Free the array of pointers to the columns
    free(mp->columns);

    // Free the remaining memory allocated for the struct
    free(mp);

    // Return
    return true;
}

/**************** map_isWall ****************/
/* Boolean return for whether a wall exists between position 1 and position 2
 * Input: pointer to a valid map and two position objects
 * Returns true if a wall exists, false otherwise
 */
bool map_isWall(map_t *mp, position_t *pos1, position_t *pos2)
{
    if ( !map_validXY(mp, position_getX(pos1), position_getY(pos1)) || !map_validXY(mp, position_getX(pos2), position_getY(pos2)) ) {
        return true;
    }

    // Get the value of the spot representing this cell-to-cell connection
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        return false;
    }
    int value = mp->columns[position_getX(index)][position_getY(index)];
    position_delete(index);

    // Check whether the value matches that of a wall, and return
    if (value != mp->wallNum) {
        return false;
    }
    return true;
}

/**************** map_isWallXY ****************/
/* XY version of map_isWall
 * Boolean return for whether a wall exists between position 1 and position 2
 * Input: pointer to a valid map and two position objects
 * Returns true if a wall exists, false otherwise
 */
bool map_isWallXY(map_t *mp, int x1, int y1, int x2, int y2)
{
    // Check validity of points
    if ( !map_validXY(mp, x1, y1) || !map_validXY(mp, x2, y2) ) {
        return true;
    } 

    // Construct position objects
    position_t* pos1 = position_new();
    position_setX(pos1, x1);
    position_setY(pos1, y1);

    position_t* pos2 = position_new();
    position_setX(pos2, x2);
    position_setY(pos2, y2);

    // Get the data position of the spot representing this cell-to-cell connection
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        position_delete(pos1);
        position_delete(pos2);
        return false;
    }

    // Get the value of the cell
    int value = mp->columns[position_getX(index)][position_getY(index)];
    position_delete(index);

    // Free memory
    position_delete(pos1);
    position_delete(pos2);

    // Check whether the value matches that of a wall, and return
    if (value != mp->wallNum) {
        return false;
    }
    return true;
}

/**************** map_setWall ****************/
/* For the positions given by the user, set the relationship between the cells to the value of 'wall'
 * Returns true if successful, false otherwise
 */
bool map_setWall(map_t *mp, position_t *pos1, position_t *pos2)
{
    // Get the position in the array
    // If the position pair given is invalid, then return false
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        return false;
    }

    // Otherwise, set the position in the array for the value of 'wall'
    mp->columns[position_getX(index)][position_getY(index)] = mp->wallNum;
    position_delete(index);
    return true;
}

/**************** map_setWallXY ****************/
/* XY version of map_setWall
 * For the positions given by the user, set the relationship between the cells to the value of 'wall'
 * Returns true if successful, false otherwise
 */
bool map_setWallXY(map_t *mp, int x1, int y1, int x2, int y2){
    
    // Convert the given Cartesian pairs to position structs
    position_t* pos1 = position_new();
    position_setX(pos1, x1);
    position_setY(pos1, y1);

    position_t* pos2 = position_new();
    position_setX(pos2, x2);
    position_setY(pos2, y2);

    // Find the data location of the relationship between the positions
    // If the location isn't valid and a NULL is returned, close data and return false
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        position_delete(pos1);
        position_delete(pos2);
        return false;
    }

    // Otherwise, set the value of the relationship between cells to 'wall'
    mp->columns[position_getX(index)][position_getY(index)] = mp->wallNum;
    position_delete(index);
        
    // Free memory
    position_delete(pos1);
    position_delete(pos2);

    return true;
}

/**************** map_isOpen ****************/
/* Boolean return for whether a wall exists between position 1 and position 2
 * Input: pointer to a valid map and two position objects
 * Returns true if the path is open, false otherwise
 */
bool map_isOpen(map_t *mp, position_t *pos1, position_t *pos2)
{

    // Get the value of the spot representing this cell-to-cell connection
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        return false;
    }

    // Get the value of the position stored at this data cell
    int value = mp->columns[position_getX(index)][position_getY(index)];
    position_delete(index);

    // Check whether the value matches that of a wall, and return
    if (value != mp->openNum) {
        return false;
    }
    return true;
}

/**************** map_isOpenXY ****************/
/* XY version of map_isOpen
 * Boolean return for whether a wall exists between position 1 and position 2
 * Input: pointer to a valid map and two position objects
 * Returns true if the path is open, false otherwise
 */
bool map_isOpenXY(map_t *mp, int x1, int y1, int x2, int y2)
{
    // Convert the Cartesian inputs into position structs
    position_t* pos1 = position_new();
    position_setX(pos1, x1);
    position_setY(pos1, y1);

    position_t* pos2 = position_new();
    position_setX(pos2, x2);
    position_setY(pos2, y2);

    // Get the value of the spot representing this cell-to-cell connection
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        position_delete(pos1);
        position_delete(pos2);
        return false;
    }

    // Get the value of the position stored at this data cell
    int value = mp->columns[position_getX(index)][position_getY(index)];
    position_delete(index);

    // Free memory
    position_delete(pos1);
    position_delete(pos2);

    // Check whether the value matches that of a wall, and return
    if (value != mp->openNum) {
        return false;
    }
    return true;
}

/**************** map_setOpen ****************/
/* For the positions given by the user, set the relationship between the cells to the value of 'open'
 * Returns true if successful, false otherwise
 */
bool map_setOpen(map_t *mp, position_t *pos1, position_t *pos2)
{
    // Get the position in the array
    // If the position pair given is invalid, then return false
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        return false;
    }

    // Otherwise, set the position in the array for the value of 'wall'
    mp->columns[position_getX(index)][position_getY(index)] = mp->openNum;
    position_delete(index);
    return true;
}


/**************** map_setOpenXY ****************/
/* XY version of map_setOpen
 * For the positions given by the user, set the relationship between the cells to the value of 'open'
 * Returns true if successful, false otherwise
 */
bool map_setOpenXY(map_t *mp, int x1, int y1, int x2, int y2)
{

    // Convert the Cartesian coordinates into position structs
    position_t* pos1 = position_new();
    position_setX(pos1, x1);
    position_setY(pos1, y1);

    position_t* pos2 = position_new();
    position_setX(pos2, x2);
    position_setY(pos2, y2);

    // Find the data location associated with the relationship between these two points
    // If the function returns NULL, indicating invalid inputs, close memory and return false
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        position_delete(pos1);
        position_delete(pos2);
        return false;
    }

    // Set the value of the data location
    mp->columns[position_getX(index)][position_getY(index)] = mp->openNum;
    position_delete(index);
    
    // Free memory
    position_delete(pos1);
    position_delete(pos2);

    return true;


}


/**************** map_isUnknown ****************/
/* Boolean return for whether a wall exists between position 1 and position 2
 * Input: pointer to a valid map and two position objects
 * Returns true if the path status is unknown, false otherwise
 */
bool map_isUnknown(map_t *mp, position_t *pos1, position_t *pos2)
{

    // Get the value of the spot representing this cell-to-cell connection
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        return false;
    }
    int value = mp->columns[position_getX(index)][position_getY(index)];
    position_delete(index);

    // Check whether the value matches that of a wall, and return
    if (value != mp->unknownNum) {
        return false;
    }
    return true;
}

/**************** map_isUnknownXY ****************/
/* XY version of map_isUnknown
 * Boolean return for whether a wall exists between position 1 and position 2
 * Input: pointer to a valid map and two position objects
 * Returns true if the path status is unknown, false otherwise
 */
bool map_isUnknownXY(map_t *mp, int x1, int y1, int x2, int y2)
{
    position_t* pos1 = position_new();
    position_setX(pos1, x1);
    position_setY(pos1, y1);

    position_t* pos2 = position_new();
    position_setX(pos2, x2);
    position_setY(pos2, y2);

    // Get the value of the spot representing this cell-to-cell connection
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        position_delete(pos1);
        position_delete(pos2);
        return false;
    }
    int value = mp->columns[position_getX(index)][position_getY(index)];
    position_delete(index);

    position_delete(pos1);
    position_delete(pos2);

    // Check whether the value matches that of a wall, and return
    if (value != mp->unknownNum) {
        return false;
    }
    return true;
}

/**************** map_setUnknown ****************/
/* For the positions given by the user, set the relationship between the cells to the value of 'unknown'
 * Returns true if successful, false otherwise
 */
bool map_setUnknown(map_t *mp, position_t *pos1, position_t *pos2)
{
    // Get the value of the spot representing this cell-to-cell connection
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        return false;
    }

    // Set the value of the data location to 'unknown'
    mp->columns[position_getX(index)][position_getY(index)] = mp->unknownNum;
    position_delete(index);
    return true;
}


/**************** map_setUnknownXY ****************/
/* XY version of setUnknown
 * For the positions given by the user, set the relationship between the cells to the value of 'unknown'
 * Returns true if successful, false otherwise
 */
bool map_setUnknownXY(map_t *mp, int x1, int y1, int x2, int y2)
{
    // Convert Cartesian coordinates into position structs
    position_t* pos1 = position_new();
    position_setX(pos1, x1);
    position_setY(pos1, y1);

    position_t* pos2 = position_new();
    position_setX(pos2, x2);
    position_setY(pos2, y2);

    // Find the data location of the relationships between data positions
    position_t *index = findDataLocation(mp, pos1, pos2);
    if (index == NULL) {
        position_delete(pos1);
        position_delete(pos2);
        return false;
    }

    // Set the value of the data location to 'unknown'
    mp->columns[position_getX(index)][position_getY(index)] = mp->openNum;
    position_delete(index);
        
    // Free temporary memory
    position_delete(pos1);
    position_delete(pos2);

    return true;
}

/**************** map_validXY ****************/
/* Checks whether the given x-y coordinate pair is a valid position in the map.
 * Returns true if yes, false otherwise
 */
bool map_validXY(map_t *mp, int x, int y) {

    // Checks whether the coordinate pair is within the bounds of the maze; returns false if not
    if (x < 0 || x > (mp->mazeWidth - 1) || y < 0 || y > (mp->mazeHeight - 1)) {
        return false;
    }
    return true;
}

/**************** map_validXY ****************/
/* Checks whether the given pair of positions are neighbors and within the bounds of the map.
 * Returns true if yes, false otherwise
 */
bool map_neighborAndValidXY(map_t *mp, int x1, int y1, int x2, int y2) {

    // If one of the points isn't valid, return false    
    if (!map_validXY(mp, x1, y1)) {
        return false;
    }

    if (!map_validXY(mp, x2, y2)) {
        return false;
    }

    // Obtain the difference between the positions in Cartesian form
    int x_difference = x2 - x1;
    int y_difference = y2 - y1;
    
    // So long as the difference is no more than 1 in any direction
    if (!(x_difference > 1 || x_difference < -1 || y_difference > 1 || y_difference < -1)) {

        // So long as the positions aren't the same
        if (!(x_difference == 0 && y_difference == 0 )) {

            // So long as if one difference is non-zero, the other is zero (cardinal neighbors)
            if ( ( x_difference == 0 && y_difference != 0) || ( x_difference != 0 && y_difference == 0) ) {
                return true;
            }

        }    
    }
    return false;
}

/**************** map_getMazeWidth ****************/
/* Returns the maze width*/
int map_getMazeWidth(map_t *mp) {
    return mp->mazeWidth;
}

/**************** findDataLocation ****************/
/* For a given map and a pair of points expressed as position structs, returns a position struct
 * containing the data location of the relationship between the positions in the map 
 * Returns a newly allocated position struct if relationship found; returns NULL if the relationship
 * was invalid.
 * Memory: Allocates fresh memory for the position struct. Caller is responsible for calling
 * position_delete() on this struct. 
 */
static position_t *findDataLocation(map_t *mp, position_t *pos1, position_t *pos2)
{
    // Check for not a neighbor pair or invalid points, and if so, print error and return NULL
    if (!map_neighborAndValidXY(mp, position_getX(pos1), position_getY(pos1), position_getX(pos2), position_getY(pos2))) {
        return NULL;
    }

    // If input tests pass, then compute the data location
    int x_index_pos2 = 2*position_getX(pos2);
    int y_index_pos2 = 2*position_getY(pos2);

    // Obtain the Cartesian difference between the two points
    int x_difference = position_getX(pos2) - position_getX(pos1);
    int y_difference = position_getY(pos2) - position_getY(pos1);

    // Obtain the map index position of the relationship between the two points (left / right 1, or up / down 1, 
    // from the second of the given points)
    int x_index = x_index_pos2 - x_difference;
    int y_index = y_index_pos2 - y_difference;

    // Create a position struct (index) to represent this index location
    position_t *index = position_new();
    position_setX(index, x_index);
    position_setY(index, y_index);
    
    // Return the index
    return index;
}

