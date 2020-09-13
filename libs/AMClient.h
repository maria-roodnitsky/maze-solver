/* ========================================================================== */
/* File: AMClient.h
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Maria Roodnitsky, Siddharth Agrawal, Alexander Hirsch
 * Component name:  AMClient
 * 
 * 
 * Date Created:    February 25, 2020
 * Last Updated:    March 8, 2020
 * 
 * Description:     AMClient.h provides a header file for the AMClient module. 
 *                  AMClient drives creation of threads to represent avatars, and 
 *                  each thread drives execution of the maze solver. The threads share
 *                  a set of data structures representing data that all avatars know,
 *                  and each avatar independently executes the algorithm to mark dead
 *                  ends, reduce the accessible area of the maze, and ultimately reach
 *                  a common destination.
 * 
 * Usage:           AMClient does not have direct usage by user. It is called by
 *                  the AMStartup module.
 * 
 */
/* ========================================================================== */
#include "AMlib.h"
#include "map.h"
#include "AMlib_avatar.h"
#include "simpleprint.h"

/** Function: client_start
 * Initializes mutex locked common datastructures and spawns thread_avatar threads 
 * @creates 
 * @param class_variables_t of server initial maze info/message
 * @return bool if mazed solved 
 */
bool client_start(class_variables_t *cv);

// TODO: Not sure if static. Depends on interthread communciation
/** Function: thread_avatar
 * Individual avatar thread, communicates with server 
 * Accesses: gameMap, done_flag
 * @param avatar_args: arguments for each thread, based off validated class_variables_t
*/
void *thread_avatar(void *avatar_args);

/************************************************
 *  client_start() Local Helper Functions 
 ***********************************************/

/** Function: thread_avatar
 * Validates that client_args is type 
 * @param client_args: initial argument, init message from server
*/
// static bool client_validate(void *client_args);

/************************************************
 *  thread_avatar() Local Helper Functions 
 ***********************************************/
/* Prints message given raw server message
 * @param uint32_t raw output from server
 */
// static void print_smessage(uint32_t raw_smsg)

/* Suggests next movement using right-hand mehtod 
 * Given: previous location, current location, previous move 
 * Returns: uint32_t move type as defined in amazing.h 
 * 1. Checks if there's a difference between current position and last position
 *    i. if there's a difference, there's a wall between previous location and previous move 
 * 2. Suggests to go right unless previous move was right and no movement
 * 3. Suggests to go in another direction  
 */
uint32_t move_for_rhr(last_move_t *last_move, map_t *amap, int current_x, int current_y);

/* Given 2 positions, returns last direction as an interger based off of:
 *  West - 0
 *  North - 1
 *  South - 2
 *  East - 3
 */
uint32_t find_last_direction(last_move_t *last_move);

/* Finds if avatar is at a designated meeting point
 * @param: int current_x: avatar's current x position
 * @param: int current_y: avatar's current y position
 * @param: int maze_width: current maze's width
 * @return: boolean if avatar is at designated meeting point (top right corner)
 * Check if avatar is at:
 * Y: 0
 * X: MAZE_WIDTH - 1
 */
bool avatar_at_point(int current_x, int current_y, int maze_width);