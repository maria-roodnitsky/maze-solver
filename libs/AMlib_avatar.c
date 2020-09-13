/* ========================================================================== */
/* File: AMlib_avatar.c
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Maria Roodnitsky, Siddharth Agrawal, Alexander Hirsch
 * Component name:  AMClient
 * 
 * 
 * Date Created:    February 25, 2020
 * Last Updated:    March 9, 2020
 * 
 * Description:     AMlib_avatar provides a library of structs relating to the notion
 *                  of an avatar. It provides position, avatar, and avatar_array structs
 *                  as well as their new, delete, getter, and setter functions.
 * 
 */
/* ========================================================================== */

// Import C Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Import project-specific libraries
#include "amazing.h"


/**************** position struct ****************/
typedef struct position {
    int x;
    int y;
} position_t;

/**************** avatar struct ****************/
typedef struct avatar {
    int avatar_ID;
    position_t *pos;
} avatar_t;

/**************** avatar_array struct ****************/
typedef struct avatar_array {
    avatar_t **avatar_array;
    int num_avatars;
} avatar_array_t;

/*** position *******************************************************************************************************/

/**************** position_new ****************/
/* Memory: returns a newly allocated position object. Caller must later free this memory
 * using position_delete
 * Initializes x and y values to dummy values (-100)
 */

position_t *position_new(void){
  position_t *position = malloc(sizeof(position_t));
  
  if (position == NULL){
    fprintf(stderr, "position_new failed to allocate memory.\n");
    exit(5);
  }
  
  position->x = -100;
  position->y = -100;
  
  return position;
}

/**************** position_delete ****************/
/* Memory: deletes the position object referred to by the pointer. Frees the memory associated with it
 */
void position_delete(position_t *pos){
  free(pos); 
}

/**************** position getters and setters ****************/
int position_getX(position_t *pos){
  return pos->x;
}

int position_getY(position_t *pos){
  return pos->y;
}

bool position_setX(position_t *pos, int x){
  pos->x = x;
  return true;
}

bool position_setY(position_t *pos, int y){
  pos->y = y;
  return true;
}


/*** avatar ********************************************************************************************************/

/**************** avatar_new ****************/
/* Allocates memory for a new avatar structure. Creates the avatar as well as a position struct
 * for the location of the avatar
 *
 */
avatar_t *avatar_new(int avatar_ID){
  avatar_t *avatar = malloc(sizeof(avatar_t));
  
    if (avatar == NULL){
    fprintf(stderr, "avatar_new failed to allocate memory.\n");
    exit(5);
  }
  
  avatar->avatar_ID = avatar_ID;
  avatar->pos = position_new();
  
  return avatar;
}

/**************** avatar_delete ****************/
/* Memory: deletes the avatar object referred to by the pointer. 
 * Internally, the function calls position_delete on the avatar's position object
 * and then frees the memory associated with the avatar pointer
 */
bool avatar_delete(avatar_t *av){
  if (av != NULL) {
    position_delete(av->pos);
    free(av);
  }
  return true;
}

/**************** avatar_getPosition ****************/
/* Memory: Does NOT allocate new memory. Returns the direct pointer to the avatar's position
 * 
 */
position_t *avatar_getPosition(avatar_t *av){
  return av->pos;
}


/**************** avatar_setPosition ****************/
/* Memory: Does NOT allocate new memory. Copies the values of the input pointer into the avatar's
 * position struct
 */
void avatar_setPosition(avatar_t *av, position_t *pos){
  av->pos->x = pos->x;
  av->pos->y = pos->y;
}

/**************** avatar getters ****************/
int avatar_getX(avatar_t *av){
  return av->pos->x;
}

int avatar_getY(avatar_t *av){
  return av->pos->y;
}

int avatar_getAvatarID(avatar_t *av){
  return av->avatar_ID;
}

/*** avatar_array ***************************************************************************************************/

/**************** avatar_array_new ****************/
/* Initializes and returns a new avatar array with capacity for num_avatars.
 * Memory: allocates fresh memory for this structure. Caller must call avatar_array_delete to free the memory
 * associated with this structure
 */
avatar_t **avatar_array_new(int num_avatars){
  avatar_t **avatar_array = malloc(sizeof(avatar_t) * num_avatars);
  
  if (avatar_array == NULL){
    fprintf(stderr, "avatar_array_new failed to allocate memory.\n");
    exit(5);
  }
  
  for (int i = 0; i < num_avatars; i++){
   avatar_array[i] = NULL;
  }
 
  return avatar_array;
}

/**************** avatar_array_add ****************/
/* Adds the given avatar to the spot in the avatar array specified by the avatar_ID contained
 * within the avatar, by copying the pointer
 */
bool avatar_array_add(avatar_t **avatar_array, avatar_t *av)
{  
  if (avatar_array[av->avatar_ID] == NULL){
    avatar_array[av->avatar_ID] = av;
    return true;
  } else {
    fprintf(stderr, "avatar_array attempting to overwrite an avatar.\n");
    return false;
  }
 
}

/**************** avatar_array_delete ****************/
/* Memory: deletes the avatar array object referred to by the pointer. 
 * Internally, the function calls avatar_delete on each avatar held by the avatar array object
 * and then frees the memory associated with the avatar array pointer
 */
bool avatar_array_delete(avatar_t **avatar_array, int num_avatar){
  for (int i = 0; i < num_avatar; i++){
    avatar_delete(avatar_array[i]);
  }
  
  free(avatar_array);
  return true;
}


// *********************************** avatar_array_find_avatar **************************************
/* Returns the lowest-numbered avatar which is present at the given position. If no avatar is present
 * at the given position, returns -1.
 * Memory: does not modify any memory, including the avatar_array passed into it.
 */
int avatar_array_find_avatar(avatar_t **avatar_array, int num_avatar, int x, int y)
{
  for (int i = 0; i < num_avatar; i++) {
    avatar_t *av = avatar_array[i];
    int av_x = avatar_getX(av);
    int av_y = avatar_getY(av);

    // If the avatar position matches the search-position, then return the avatar ID
    if ( av_x == x && av_y == y) {
      return avatar_getAvatarID(av);
    }
  }
  
  return -1;
}