/* ========================================================================== */
/* File: AMlib.h
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Siddharth Agrawal, Alexander Hirsch, Maria Roodnitsky
 * Component name:  AMlib.h
 *
 * Date Created:    February 25, 2020
 * Last Updated:    March 8, 2020
 * 
 * This file exports threee data structures, as well as their supporting new, 
 * delete, get, and set functions:
 *  - class_variables
 *  - thread_initial_info
 *  - last_move
 *
 * Memory: memory management is handled by new and delete functions. Getters and setters
 * do not allocate memory. When input or return is a pointer, the pointer saved or returned 
 * is to the same data as the original allocation
 */
/* ========================================================================== */

// Import C Standard libraries
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Import project-specific libraries
#include "amazing.h"
#include "AMlib_avatar.h"
#include "map.h"

/*** Structures Exported *********************************************************************************************************/
typedef struct class_variables class_variables_t;
typedef struct thread_initial_info thread_initial_info_t;
typedef struct last_move last_move_t;

/*** Functions for class_variables ***********************************************************************************************/

/**************** class_variables_new ****************/
/* Allocates memory for a class_variables struct, and writes the inputs into the struct 
 * Memory: allocates memory for the struct. Caller is responsible for calling class_variables_delete
 * to delete the struct
 */
class_variables_t *class_variables_new(int num_avatars, int difficulty_level, const char *hostname);

/**************** class_variables_delete ****************/
/* Frees all memory associated with class_variables. 
 * For dynamically allocated strings inside of class_variables struct, if they are not NULL, they are free'd
 * These are:
 * - log_file_name
 * Note:
 * - Hostname is NOT free'd, because it is being assigned directly from input arguments (consts)
 */
bool class_variables_delete(class_variables_t *cv);

/**************** class_variables getters and setters ****************/
int class_variables_get_MazeHeight(class_variables_t *cv);
int class_variables_get_MazeWidth(class_variables_t *cv);
int class_variables_get_num_avatars(class_variables_t *cv);
const char *class_variables_get_hostname(class_variables_t *cv);
int class_variables_get_difficulty(class_variables_t *cv);
int class_variables_get_mazePort(class_variables_t *cv);
const char *class_variables_get_log_file_name(class_variables_t *cv);
void class_variables_set_MazeWidth(class_variables_t *cv, int MazeWidth);
void class_variables_set_MazeHeight(class_variables_t *cv, int MazeHeight);
void class_variables_set_num_avatars(class_variables_t *cv, int num_avatars);
void class_variables_set_difficulty(class_variables_t *cv, int difficulty);
void class_variables_set_hostname(class_variables_t *cv, const char *hostname);
void class_variables_set_mazePort(class_variables_t *cv, int mazePort);
void class_variables_set_log_file_name(class_variables_t *cv, char *name);

/*** Functions for thread_initial_info ******************************************************************************************/

/**************** thread_initial_info_new ****************/
/* Allocates memory for a thread_initial_info struct and assigns its fields using
 * pointers from class_variables input, as well as thread_id (id). SOT variables
 * are left NULL for later assignment by setters. 
 * Caller is responsible for later calling thread_initial_info_delete to free
 * the memory associated with the struct. 
 */
thread_initial_info_t *thread_initial_info_new(class_variables_t *class_v, int id);

/**************** thread_initial_info_delete ****************/
/* Frees the memory associated with thread_initial_info. Thread_initial_info
 * only stores pointers to variables deleted by the deleters for other structs;
 * therefore, this deleter only frees the memory for the thread_initial_info struct
 */
void thread_initial_info_delete(thread_initial_info_t *tii);

/**************** thread_initial_info getters and setters ****************/
int thread_initial_info_get_mazePort(thread_initial_info_t *tii);
const char *thread_initial_info_get_hostName(thread_initial_info_t *tii);
int thread_initial_info_get_threadID(thread_initial_info_t *tii);
int thread_initial_info_get_mazePort(thread_initial_info_t *tii);
const char *thread_initial_info_get_hostName(thread_initial_info_t *tii);
int thread_initial_info_get_threadID(thread_initial_info_t *tii);
const char *thread_initial_info_get_log_file_name(thread_initial_info_t *tii);
int thread_initial_info_get_num_avatars(thread_initial_info_t *tii);
int thread_initial_info_get_difficulty(thread_initial_info_t *tii);
int thread_initial_info_get_MazeWidth(thread_initial_info_t *tii);
int thread_initial_info_get_MazeHeight(thread_initial_info_t *tii);
void thread_initial_info_set_mazePort(thread_initial_info_t *tii, int mazePort);
void thread_initial_info_set_hostName(thread_initial_info_t *tii, char *hostname);
void thread_initial_info_set_threadID(thread_initial_info_t *tii, int threadID);
void thread_initial_info_set_log_file_name(thread_initial_info_t *tii, char *name);
void thread_initial_info_set_num_avatars(thread_initial_info_t *tii, int num_avatars);
void thread_initial_info_set_difficulty(thread_initial_info_t *tii, int difficulty);
void thread_initial_info_set_MazeWidth(thread_initial_info_t *tii, int mazeWidth);
void thread_initial_info_set_MazeHeight(thread_initial_info_t *tii, int mazeHeight);
void thread_initial_info_set_SOT_shared_map(thread_initial_info_t *tii, map_t *mp);
map_t *thread_initial_info_get_SOT_shared_map(thread_initial_info_t *tii);
void thread_initial_info_set_SOT_last_move_global(thread_initial_info_t *tii, last_move_t *SOT_last_move_global);
last_move_t *thread_initial_info_get_SOT_last_move_global(thread_initial_info_t *tii);
void thread_initial_info_set_SOT_avatar_array(thread_initial_info_t *tii, avatar_t **avatar_array);
avatar_t **thread_initial_info_get_SOT_avatar_array(thread_initial_info_t *tii);

/*** Functions for last_move *****************************************************************************************************/

/**************** last_move_new ****************/
/* last_move_new allocates memory for a last_move structure, and it
 * initializes all values to -100. These values can be set to
 * requirement using the struct's setters. Caller is responsible 
 * for calling last_move_delete to free the memory in this struct
 */
last_move_t *last_move_new();

/**************** last_move_delete ****************/
/* frees the last_move_delete struct
 *
 */
bool last_move_delete(last_move_t *last_move);

/**************** last_move getters and setters ****************/
int last_move_get_initial_x(last_move_t *last_move);
int last_move_get_initial_y(last_move_t *last_move);
int last_move_get_x_attempt(last_move_t *last_move);
int last_move_get_y_attempt(last_move_t *last_move);
int last_move_get_last_ID(last_move_t *last_move);
int last_move_get_last_success_dir(last_move_t *last_move);
void last_move_set_initial_x(last_move_t *last_move, int initial_x);
void last_move_set_initial_y(last_move_t *last_move, int initial_y);
void last_move_set_x_attempt(last_move_t *last_move, int x_attempt);
void last_move_set_y_attempt(last_move_t *last_move, int y_attempt);
void last_move_set_last_ID(last_move_t *last_move, int last_ID);
void last_move_set_last_success_dir(last_move_t *last_move, int last_dir);
