/* ========================================================================== */
/* File: AMlib.c
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Siddharth Agrawal, Alexander Hirsch, Maria Roodnitsky
 * Component name:  AMlib.c
 *
 * Date Created:    February 25, 2020
 * Last Updated:    March 8, 2020
 * 
 * This file implements the functions in AMlib.h. It provides threee data structures,
 * as well as their supporting new, delete, get, and set functions:
 *  - class_variables
 *  - thread_initial_info
 *  - last_move
 *
 */
/* ========================================================================== */

// Import C Standard libraries
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Import project-specific libraries
#include "amazing.h"
#include "AMlib.h"
#include "AMlib_avatar.h"
#include "map.h"

/**************** class_variables_struct ****************/
typedef struct class_variables
{
    int num_avatars;      // Provided by user
    int difficulty_level; // Provided by user
    const char *hostname; // Provided by user
    int mazePort;         // Provided by server
    int mazeWidth;        // Provided by server
    int mazeHeight;       // Provided by server
    char *log_file_name;  // Constructed by this program
} class_variables_t;

/**************** class_variables_new ****************/
/* Allocates memory for a class_variables struct, and writes the inputs into the struct 
 * Memory: allocates memory for the struct. Caller is responsible for calling class_variables_delete
 * to delete the struct
 */
class_variables_t *class_variables_new(int num_avatars, int difficulty_level, const char *hostname)
{
    class_variables_t *new_class_variables = malloc(sizeof(class_variables_t) + 10);

    new_class_variables->num_avatars = num_avatars;
    new_class_variables->difficulty_level = difficulty_level;
    new_class_variables->hostname = hostname;
    new_class_variables->log_file_name = NULL;

    return (new_class_variables);
}

/**************** class_variables_delete ****************/
/* Frees all memory associated with class_variables. 
 * For dynamically allocated strings inside of class_variables struct, if they are not NULL, they are free'd
 * These are:
 * - log_file_name
 * Note:
 * - Hostname is NOT free'd, because it is being assigned directly from input arguments (consts)
 */
bool class_variables_delete(class_variables_t *cv)
{
    if (cv != NULL)
    {
        if (cv->log_file_name != NULL)
        {
            free(cv->log_file_name);
        }
        free(cv);
    }
    return true;
}

/**************** class_variables getters and setters ****************/
int class_variables_get_MazeWidth(class_variables_t *cv)
{
    return cv->mazeWidth;
}

int class_variables_get_MazeHeight(class_variables_t *cv)
{
    return cv->mazeHeight;
}
int class_variables_get_num_avatars(class_variables_t *cv)
{
    return cv->num_avatars;
}

int class_variables_get_difficulty(class_variables_t *cv)
{
    return cv->difficulty_level;
}

int class_variables_get_mazePort(class_variables_t *cv)
{
    return (cv->mazePort);
}
const char *class_variables_get_log_file_name(class_variables_t *cv)
{
    return cv->log_file_name;
}

void class_variables_set_MazeWidth(class_variables_t *cv, int MazeWidth)
{
    cv->mazeWidth = MazeWidth;
}

void class_variables_set_MazeHeight(class_variables_t *cv, int MazeHeight)
{
    cv->mazeHeight = MazeHeight;
}
void class_variables_set_num_avatars(class_variables_t *cv, int num_avatars)
{
    cv->num_avatars = num_avatars;
}

void class_variables_set_difficulty(class_variables_t *cv, int difficulty)
{
    cv->difficulty_level = difficulty;
}

void class_variables_set_mazePort(class_variables_t *cv, int mazePort)
{
    cv->mazePort = mazePort;
}

void class_variables_set_log_file_name(class_variables_t *cv, char *name)
{
    cv->log_file_name = name;
}

void class_variables_set_hostname(class_variables_t *cv, const char *hostname)
{
    cv->hostname = hostname;

}

const char *class_variables_get_hostname(class_variables_t *cv)
{
    return cv->hostname;
}

/**************** thread_initial_info struct ****************/
typedef struct thread_initial_info
{
    int num_avatars;                    // Provided by user
    int difficulty_level;               // Provided by user
    const char *hostname;               // Provided by user
    int mazePort;                       // Provided by server
    int mazeWidth;                      // Provided by server
    int mazeHeight;                     // Provided by server
    char *log_file_name;                // Constructed by this program
    map_t *SOT_shared_map;              // Constructed by this program
    last_move_t *SOT_last_move_global;  // Constructed by this program
    avatar_t **SOT_avatar_array;        // Constructed by this program
    int threadID;                       // Constructed by this program

} thread_initial_info_t;

/**************** thread_initial_info_new ****************/
/* Allocates memory for a thread_initial_info struct and assigns its fields using
 * pointers from class_variables input, as well as thread_id (id). SOT variables
 * are left NULL for later assignment by setters. 
 * Caller is responsible for later calling thread_initial_info_delete to free
 * the memory associated with the struct. 
 */
thread_initial_info_t *thread_initial_info_new(class_variables_t *class_v, int id)
{
    thread_initial_info_t *new_initial = malloc(sizeof(thread_initial_info_t) + 10);
    assert(new_initial);
    new_initial->num_avatars = class_v->num_avatars;           // Provided by user
    new_initial->difficulty_level = class_v->difficulty_level; // Provided by user
    new_initial->hostname = class_v->hostname;                 // Provided by user
    new_initial->mazePort = class_v->mazePort;                 // Provided by server
    new_initial->mazeWidth = class_v->mazeWidth;               // Provided by server
    new_initial->mazeHeight = class_v->mazeHeight;             // Provided by server
    new_initial->log_file_name = class_v->log_file_name;       // Constructed by this program
    new_initial->threadID = id;                                // Constructed by this program
    new_initial->SOT_shared_map = NULL;                        // Constructed by this program
    new_initial->SOT_last_move_global = NULL;                  // Constructed by this program
    new_initial->SOT_avatar_array = NULL;                      // Constructed by this program
    return new_initial;
}

/**************** thread_initial_info_delete ****************/
/* Frees the memory associated with thread_initial_info. Thread_initial_info
 * only stores pointers to variables deleted by the deleters for other structs;
 * therefore, this deleter only frees the memory for the thread_initial_info struct
 */
void thread_initial_info_delete(thread_initial_info_t *tii) {
    free(tii);
}

/**************** thread_initial_info getters and setters ****************/
void thread_initial_info_set_SOT_shared_map(thread_initial_info_t *tii, map_t *mp)
{
    tii->SOT_shared_map = mp;
}

map_t *thread_initial_info_get_SOT_shared_map(thread_initial_info_t *tii)
{
    return tii->SOT_shared_map;
}

void thread_initial_info_set_SOT_last_move_global(thread_initial_info_t *tii, last_move_t *SOT_last_move_global)
{
    tii->SOT_last_move_global = SOT_last_move_global;
}

last_move_t *thread_initial_info_get_SOT_last_move_global(thread_initial_info_t *tii)
{
    return tii->SOT_last_move_global;
}

void thread_initial_info_set_SOT_avatar_array(thread_initial_info_t *tii, avatar_t **avatar_array)
{
    tii->SOT_avatar_array = avatar_array;
}

avatar_t **thread_initial_info_get_SOT_avatar_array(thread_initial_info_t *tii)
{
    return tii->SOT_avatar_array;
}

int thread_initial_info_get_mazePort(thread_initial_info_t *tii)
{
    return tii->mazePort;
}

const char *thread_initial_info_get_hostName(thread_initial_info_t *tii)
{
    return tii->hostname;
}

int thread_initial_info_get_threadID(thread_initial_info_t *tii)
{
    return tii->threadID;
}

const char *thread_initial_info_get_log_file_name(thread_initial_info_t *tii)
{
    return tii->log_file_name;
}

int thread_initial_info_get_num_avatars(thread_initial_info_t *tii)
{
    return tii->num_avatars;
}
int thread_initial_info_get_difficulty(thread_initial_info_t *tii)
{
    return tii->difficulty_level;
}
int thread_initial_info_get_MazeWidth(thread_initial_info_t *tii)
{
    return tii->mazeWidth;
}
int thread_initial_info_get_MazeHeight(thread_initial_info_t *tii)
{
    return tii->mazeHeight;
}

void thread_initial_info_set_mazePort(thread_initial_info_t *tii, int mazePort)
{
    tii->mazePort = mazePort;
}

void thread_initial_info_set_hostName(thread_initial_info_t *tii, char *hostname)
{
    tii->hostname = hostname;
}

void thread_initial_info_set_threadID(thread_initial_info_t *tii, int threadID)
{
    tii->threadID = threadID;
}

void thread_initial_info_set_log_file_name(thread_initial_info_t *tii, char *name)
{
    tii->log_file_name = name;
}

void thread_initial_info_set_num_avatars(thread_initial_info_t *tii, int num_avatars)
{
    tii->num_avatars = num_avatars;
}
void thread_initial_info_set_difficulty(thread_initial_info_t *tii, int difficulty)
{
    tii->difficulty_level = difficulty;
}
void thread_initial_info_set_MazeWidth(thread_initial_info_t *tii, int mazeWidth)
{
    tii->mazeWidth = mazeWidth;
}
void thread_initial_info_set_MazeHeight(thread_initial_info_t *tii, int mazeHeight)
{
    tii->mazeHeight = mazeHeight;
}

/**************** last_move struct ****************/
/* struct which holds the last attempted move by an avatar 
 */
typedef struct last_move
{
    int avatar_attempted_ID;
    int initial_x;
    int initial_y;
    int x_attempt;
    int y_attempt;
    int last_success_dir;
} last_move_t;


/**************** last_move_new ****************/
/* last_move_new allocates memory for a last_move structure, and it
 * initializes all values to -100. These values can be set to
 * requirement using the struct's setters. Caller is responsible 
 * for calling last_move_delete to free the memory in this struct
 */
last_move_t *last_move_new()
{
    last_move_t *last_move = calloc(1, sizeof(last_move_t));
    last_move->avatar_attempted_ID = -100;
    last_move->initial_x = -100;
    last_move->initial_y = -100;
    last_move->x_attempt = -100;
    last_move->y_attempt = -100;
    return last_move;
}

/**************** last_move_delete ****************/
/* frees the last_move_delete struct
 *
 */
bool last_move_delete(last_move_t *last_move)
{
    if (last_move != NULL) {
        free(last_move);
    }
    return true;
}


/**************** last_move getters and setters ****************/
/* Getters and setters for the last_move struct. 
 *
 */

int last_move_get_initial_x(last_move_t *last_move)
{
    return last_move->initial_x;
}

int last_move_get_initial_y(last_move_t *last_move)
{
    return last_move->initial_y;
}
int last_move_get_x_attempt(last_move_t *last_move)
{
    return last_move->x_attempt;
}
int last_move_get_y_attempt(last_move_t *last_move)
{
    return last_move->y_attempt;
}
int last_move_get_last_ID(last_move_t *last_move)
{
    return last_move->avatar_attempted_ID;
}

int last_move_get_last_success_dir(last_move_t *last_move)
{
    return last_move->last_success_dir;
}

void last_move_set_initial_x(last_move_t *last_move, int initial_x)
{
    last_move->initial_x = initial_x;
}

void last_move_set_initial_y(last_move_t *last_move, int initial_y)
{
    last_move->initial_y = initial_y;
}
void last_move_set_x_attempt(last_move_t *last_move, int x_attempt)
{
    last_move->x_attempt = x_attempt;
}
void last_move_set_y_attempt(last_move_t *last_move, int y_attempt)
{
    last_move->y_attempt = y_attempt;
}
void last_move_set_last_ID(last_move_t *last_move, int last_ID)
{
    last_move->avatar_attempted_ID = last_ID;
}

void last_move_set_last_success_dir(last_move_t *last_move, int last_dir)
{
    last_move->last_success_dir = last_dir;
}
