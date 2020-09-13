/* ========================================================================== */
/* File: AMlib.h
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Siddharth Agrawal, Alexander Hirsch, Maria Roodnitsky
 * Component name:  map
 *
 * Date Created:    February 25, 2020
 *
 * This file exports the following structs for general use:
 *   * position
 *   * avatar
 *   * avatar_array
 * 
 * STATUS: position and avatar are implemented. Avatar_array is to be implemented
 */
/* ========================================================================== */


// Import C Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Import project-specific libraries
#include "amazing.h"


typedef struct position position_t;
typedef struct avatar avatar_t;
typedef struct avatar_array avatar_array_t;

// *********************************** position **************************************

/**************** position_new ****************/
/* Memory: returns a newly allocated position object. Caller must later free this memory
 * using position_delete
 * Initializes x and y values to dummy values (-100)
 */
position_t *position_new(void);

/**************** position_delete ****************/
/* Memory: deletes the position object referred to by the pointer. Frees the memory associated with it
 */
void position_delete(position_t *pos);

// Getters and setters for the position struct
int position_getX(position_t *pos);
int position_getY(position_t *pos);
bool position_setX(position_t *pos, int x);
bool position_setY(position_t *pos, int y);



// *********************************** avatar **************************************

/**************** avatar_new ****************/
avatar_t *avatar_new(int avatar_ID);

/**************** avatar_delete ****************/
/* Memory: deletes the avatar object referred to by the pointer. 
 * Internally, the function calls position_delete on the avatar's position object
 * and then frees the memory associated with the avatar pointer
 */
bool avatar_delete(avatar_t *av);

/**************** avatar_getPosition ****************/
/* Memory: Does NOT allocate new memory. Returns the direct pointer to the avatar's position
 * 
 */
position_t *avatar_getPosition(avatar_t *av);

/**************** avatar_setPosition ****************/
/* Memory: Does NOT allocate new memory. Copies the values of the input pointer into the avatar's
 * position struct
 */
void avatar_setPosition(avatar_t *av, position_t *pos);


// Getters
int avatar_getX(avatar_t *av);
int avatar_getY(avatar_t *av);
int avatar_getAvatarID(avatar_t *av);

// *********************************** avatar_array **************************************

/**************** avatar_array_new ****************/
/* Initializes and returns a new avatar array with capacity for num_avatars.
 * Memory: allocates fresh memory for this structure. Caller must call avatar_array_delete to free the memory
 * associated with this structure
 */
avatar_t **avatar_array_new(int num_avatars);

/**************** avatar_array_delete ****************/
/* Memory: deletes the avatar array object referred to by the pointer. 
 * Internally, the function calls avatar_delete on each avatar held by the avatar array object
 * and then frees the memory associated with the avatar array pointer
 */
bool avatar_array_delete(avatar_t **avatar_array, int num_avatar);

/**************** avatar_array_add ****************/
/* Adds the given avatar to the spot in the avatar array specified by the avatar_ID contained
 * within the avatar, by copying the pointer
 */
bool avatar_array_add(avatar_t **avatar_array, avatar_t *av);

// *********************************** avatar_array_find_avatar **************************************
/* Returns the lowest-numbered avatar which is present at the given position. If no avatar is present
 * at the given position, returns -1.
 * Memory: does not modify any memory, including the avatar_array passed into it.
 */
int avatar_array_find_avatar(avatar_t **avatar_array, int num_avatar, int x, int y);

