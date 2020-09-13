/* ========================================================================== */
/* File: AMClient.c
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Maria Roodnitsky, Siddharth Agrawal, Alexander Hirsch
 * Component name:  AMClient
 * 
 * 
 * Date Created:    February 25, 2020
 * Last Updated:    March 7, 2020
 * 
 * Description:     AMClient drives creation of threads to represent avatars, and 
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

// Import C Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <netdb.h>
#include <netinet/in.h>

// Import project-specific libraries
#include "map.h"
#include "AMlib_avatar.h"
#include "AMlib.h"
#include "amazing.h"
#include "AMClient.h"
#include "simpleprint.h"

/**************** Debug Switches ****************/
static const int DEBUG_SWITCH_ITR = 0;                                         // DEBUG_SWITCH_ITR: on = 1, off = 0
static const int END_RUN_ITR = 100;                                            // If the DEBUG_SWITCH_ITR is on, thread execution will exit after END_RUN_ITR iterations

/**************** Mutual Exclusion Locks ****************/
pthread_mutex_t mutexReadAndWrite = PTHREAD_MUTEX_INITIALIZER;          // Mutex lock used for all reads and writes to the server

/**************** Logging Step: file-local constants ****************/
// Set of special constants used locally by the threads to save results of update step for the logging step
static const int prev_move_wall        = 1;                             // Indicates that the prior move was not successful and resulted in encountering a wall
static const int prev_move_path        = 2;                             // Indicates that the prior move was successful and traversed a path
static const int prev_move_path_fill   = 3;                             // Indicates that the prior move was successful and traversed a path, and that a trap was identified
                                                                        // and a wall was filled



/**************** client_start ****************/
/* client_start is the parent which spawns each of the player threads. It is called from main() in AMStartup.c
 * Memory: Takes the class_variables struct as a pointer from AMStartup.c containing initialization parameters
 * Does not free this memory at the end of the run; freeing class_variables_t *cv is the responsibility of the
 * caller program (AMStartup).
 * 
 * The function does the following:
 *  - initializes structures shared by all of the threads (e.g. map)
 *  - creates a struct (thread_initial_info) to pass to each thread containing useful information (e.g. pointers to shared data structures, 
 *    and the thread's ID)
 *  - spawns the threads
 *  - upon closing of each thread, it frees memory which was allocated within this function
 * 
 * Return:
 *  - false if any error occurs that should terminate the program
 *  - true otherwise
 * 
 */
bool client_start(class_variables_t *cv)
{
    /*** 1. Initialize Parent Scope (Source of truth) data structures ***/

    // SOT_shared_map
    map_t *SOT_shared_map;
    SOT_shared_map = map_new(class_variables_get_MazeWidth(cv), class_variables_get_MazeHeight(cv));

    // SOT_Last_move_global
    last_move_t *SOT_last_move_global;
    SOT_last_move_global = last_move_new();

    // SOT_avatar_array
    avatar_t **SOT_avatar_array = avatar_array_new(class_variables_get_num_avatars(cv));

    /*** 2. Spawn threads ***/

    // Create array of avatar threads
    pthread_t client_threads[class_variables_get_num_avatars(cv) - 1];

    // For each thread to be created ...
    for (int i = 0; i < class_variables_get_num_avatars(cv); i++)
    {

        // Create a struct holding values to pass to the thread
        thread_initial_info_t *thread_info = thread_initial_info_new(cv, i);
        thread_initial_info_set_SOT_shared_map(thread_info, SOT_shared_map);
        thread_initial_info_set_SOT_last_move_global(thread_info, SOT_last_move_global);
        thread_initial_info_set_SOT_avatar_array(thread_info, SOT_avatar_array);

        // Create the thread
        int return_value = pthread_create(&client_threads[i], NULL, thread_avatar, thread_info);
        if (return_value != 0)
        {
            fprintf(stderr, "Error, could not spawn thread %d. Returning from client_start function with 'false' return value.\n", i);
            return false;
        }
        printf("STATUS: Client Start: Thread #%d spawned.\n", i);
    }

    /*** 3. Join the parent to the children threads, so that the parent will not close until the threads close ***/
    for (int i = 0; i < class_variables_get_num_avatars(cv); i++)
    {
        pthread_join(client_threads[i], NULL);
    }

    /*** 4. Once the threads have closed, close out data structures shared by all threads ***/
    map_delete(SOT_shared_map);
    last_move_delete(SOT_last_move_global);
    avatar_array_delete(SOT_avatar_array, class_variables_get_num_avatars(cv));

    // 5. Return success
    return true;
}

/**************** thread_avatar ****************/
/* thread_avatar is the primary driver function for each thread, representing a player.
 * Memory: Accepts a thread_initial_info object as parameter. 
 * TO DO: The function needs to call a thread_initial_info_delete before exiting.
 * TO DO: Need to learn how to manage memory when exiting from a thread. 
 *
 * The function does the following:
 * - Establishes a connection with the server
 * - Sends an AM_AVATAR_READY signal to the server
 * - Enters a while (1) loop. This while loop does the following:
 *    - Reads the latest message from the server
 *    - If the message is not an AM_AVATAR_TURN message, handles it appropriately
 *    - If the message is an AM_AVATAR_TURN message, but it is not the thread's turn, then 
 *      proceeds to the next iteration without taking any action
 *    - If the message is an AM_AVATAR_TURN message, and it is the thread's turn, then
 *       - Prints the current positions (CURRENT)
 *       - Saves the impact of the message to all relevant data structures (FUTURE)
 *       - Runs decision algorithm to determine next move (FUTURE)
 *       - Constructs a move north message (CURRENT - TEMP)
 *       - Calls print and logging (FUTURE)
 *       - Sends a move message to the server (CURRENT)
 * 
 */

void *thread_avatar(void *avatar_args)
{

    /*** 1. Save Initial Variables ***/

    // Save down the input argument (thread_initial_info) and pull out the thread_id for use
    thread_initial_info_t *thread_info = (thread_initial_info_t *)avatar_args;
    int thread_id = thread_initial_info_get_threadID(thread_info);

    // Set pointer for a thread-scope (Scope 2) last_thread representing the thread's last successful move
    last_move_t *last_thread_success_move;

    // Declare local positional variables
    int local_current_x;
    int local_current_y;
    int local_attempted_x;
    int local_attempted_y;

    /*** 2. Establish Server Connection ***/

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        fprintf(stderr, "error opening socket");
        pthread_exit(0);
    }

    // Initialize fields
    // create appropriate socket address struct
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(thread_initial_info_get_mazePort(thread_info));

    // create host struct, thereby validating given hostname
    struct hostent *host = gethostbyname(thread_initial_info_get_hostName(thread_info)); // server hostname
    if (host == NULL)
    {
        fprintf(stderr, "Error 5: unknown host\n");
        pthread_exit(0);
    }
    memcpy(&servaddr.sin_addr, host->h_addr_list[0], host->h_length);

    // connect to server, checking success
    if ((connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
    {
        fprintf(stderr, "Error connecting to server\n");
        pthread_exit(0);
    }

    /*** 3. Send Client Ready Message to the Server ***/

    // Lock the write section
    pthread_mutex_lock(&mutexReadAndWrite);

    AM_Message ready_message;
    memset(&ready_message, 0, sizeof(ready_message));
    ready_message.type = htonl(AM_AVATAR_READY);
    ready_message.avatar_ready.AvatarId = htonl(thread_id);
    if ((write(sock, &ready_message, sizeof(ready_message))) < 0)
    {
        fprintf(stderr, "Error writing ready message to server\n");
        pthread_exit(0);
    }
    printf("Thread #%d: Ready message sent to server \n", thread_id);

    // Unlock the write section
    pthread_mutex_unlock(&mutexReadAndWrite);
    sleep(2);

    /*** 4. Enter the Primary While Loop's Control ***/
    int iteration_count = 0;
    last_thread_success_move = last_move_new();
    while (1)
    {

        /*** 1. Read input from the server ***/

        // Prepare a message to accept the read
        AM_Message return_message;
        memset(&return_message, 0, sizeof(return_message));

        // Conduct the read and throw an error if any problem in reading
        int bytesRead;
        if ((bytesRead = read(sock, &return_message, sizeof(return_message))) < 0)
        {
            fprintf(stderr, "\tError reading from server\n");
            exit(7);
        }

        /*** 2. Begin mutex lock over remainder of while-loop iteration ***/
        pthread_mutex_lock(&mutexReadAndWrite);

        /*** 3. Handle message types other than AM_AVATAR_TURN ***/

        // Set up for updating the map, in case the game is solved on this turn
        avatar_t **avatar_array = thread_initial_info_get_SOT_avatar_array(thread_info);
        last_move_t *SOT_last_move_global = thread_initial_info_get_SOT_last_move_global(thread_info);
        int previous_move_code = -1;

        // If the game is solved:
        if (ntohl(return_message.type) == AM_MAZE_SOLVED)
        {
            // Only Thread 0 should write the solved message to the log
            if (thread_id == 0)
            {
                if (iteration_count != 0) 
                {
                    
                    // *** Special case handling for the last move ***/
                    
                    // Pull out the values of the global last_move
                    int last_id = last_move_get_last_ID(SOT_last_move_global);
                    int attempted_x = last_move_get_x_attempt(SOT_last_move_global);
                    int attempted_y = last_move_get_y_attempt(SOT_last_move_global);
                    int initial_x = last_move_get_initial_x(SOT_last_move_global);
                    int initial_y = last_move_get_initial_y(SOT_last_move_global);

                    // Because the maze was solved, this move must have succeeded. Record the previous_move_code for passing to logging
                    previous_move_code = prev_move_path;

                    // In the avatar array, update the position of the prior avatar
                    position_setX(avatar_getPosition(avatar_array[last_id]), attempted_x);
                    position_setY(avatar_getPosition(avatar_array[last_id]), attempted_y);

                    // Wall-filler: this fills in traps identified by the previous thread.
                    int wall_count = 0;
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x + 1, initial_y, initial_x, initial_y)){wall_count++;}
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x, initial_y + 1, initial_x, initial_y)){wall_count++;}
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x - 1, initial_y, initial_x, initial_y)){wall_count++;}
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x, initial_y - 1, initial_x, initial_y)){wall_count++;}
                    if (wall_count > 2) {
                        if (avatar_array_find_avatar(thread_initial_info_get_SOT_avatar_array(thread_info), 
                        thread_initial_info_get_num_avatars(thread_info), initial_x, initial_y) == -1) {
                            
                            // Set the wall
                            map_setWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x, initial_y, attempted_x, attempted_y);

                            // Update the previous_move_code for passing to logging
                            previous_move_code = prev_move_path_fill;
                        }
                    }
                }

                // System call to clear the screen before ASCII print
                // Acknowledgement: We learned to clear the screen from the following article: 
                // https://stackoverflow.com/questions/2347770/how-do-you-clear-the-console-screen-in-c
                system("@cls||clear");  
                
                // Print the ASCII map
                print_map(
                    thread_initial_info_get_MazeHeight(thread_info),
                    thread_initial_info_get_MazeWidth(thread_info),
                    thread_initial_info_get_SOT_avatar_array(thread_info),
                    thread_initial_info_get_num_avatars(thread_info),
                    thread_initial_info_get_SOT_shared_map(thread_info));

                // Open the log file and save the AM_SOLVED message contents
                FILE *fp;
                fp = fopen(thread_initial_info_get_log_file_name(thread_info), "a");
                if (fp != NULL) { 
                    // Save the message
                    fprintf(fp, "\n*** Received AM_MAZE_SOLVED ***\n");
                    fprintf(fp, "Message contents: Num avatars: %d; Difficulty level: %d; Num moves: %d; Hash: %d\n",
                            ntohl(return_message.maze_solved.nAvatars), ntohl(return_message.maze_solved.Difficulty),
                            ntohl(return_message.maze_solved.nMoves), ntohl(return_message.maze_solved.Hash));

                    // Close the log file
                    fclose(fp);
                }

            }

            // Unlock and break loop
            pthread_mutex_unlock(&mutexReadAndWrite);
            break;
        }

        // If an error message is detected:
        if (IS_AM_ERROR(ntohl(return_message.type)))
        {
            
            FILE *fp;
            fp = fopen(thread_initial_info_get_log_file_name(thread_info), "a");
            if (fp != NULL) 
            {
                // Write both to log file and to screen
                fprintf(fp, "\tThread #%d: Received error. Message type: %d \n", thread_id, ntohl(return_message.type));
                fprintf(stdout, "\tThread #%d: Received error. Message type: %d \n", thread_id, ntohl(return_message.type));

                // Handlers for specific error messages - 

                // If the server is out of memory, close the thread gracefully and exit
                if (ntohl(return_message.type) == AM_SERVER_OUT_OF_MEM)
                {
                    fprintf(fp, "\tThread #%d: The error message type is AM_SERVER_OUT_OF_MEM \n", thread_id);
                    fprintf(stdout, "\tThread #%d: The error message type is AM_SERVER_OUT_OF_MEM \n", thread_id);
                }
                // If the server has timed out, close the thread gracefully and exit
                if (ntohl(return_message.type) == AM_SERVER_TIMEOUT)
                {
                    fprintf(fp, "\tThread #%d: The error message type is AM_SERVER_TIMEOUT \n", thread_id);
                    fprintf(stdout, "\tThread #%d: The error message type is AM_SERVER_TIMEOUT \n", thread_id);
                }
                // If there have been too many moves, close the thread gracefully and exit
                if (ntohl(return_message.type) == AM_TOO_MANY_MOVES)
                {
                    fprintf(fp, "\tThread #%d: The error message type is AM_TOO_MANY_MOVES \n", thread_id);
                    fprintf(stdout, "\tThread #%d: The error message type is AM_TOO_MANY_MOVES \n", thread_id);
                }
                // If the server disk quota has been exceeded, close the thread gracefully and exit
                if (ntohl(return_message.type) == AM_SERVER_DISK_QUOTA){
                    fprintf(fp, "\tThread #%d: The error message type is AM_SERVER_DISK_QUOTA \n", thread_id);
                    fprintf(stdout, "\tThread #%d: The error message type is AM_SERVER_DISK_QUOTA \n", thread_id);

                }
                fclose(fp);
            }

            // Close variables
            free(avatar_args);
            last_move_delete(last_thread_success_move);
            pthread_mutex_unlock(&mutexReadAndWrite);

            // Exit
            return NULL;
        }

        /*** 5. Further execution of this iteration only if ... ***/

        // If it is an AM_AVATAR_TURN message and it is this avatar's turn ...
        if (ntohl(return_message.type) == AM_AVATAR_TURN && ntohl(return_message.avatar_turn.TurnId) == thread_id)
        { 

            /*** 1. Save the impact of the message to all relevant data structures ***/
            // ALL CHANGES TO SHARED DATA STRUCTURES SHOULD OCCUR BETWEEN HERE -->
            // Check if last move was succesful
            // if succesful,
            // - update last successful direction in thread_last_move
            // - update the map
            // - update the avatar_array


            // If the last_thread_success_move has not been initialized beyond calling new (i.e., ID = -100), then initialize it
            if (last_move_get_last_ID(last_thread_success_move) == -100) {

                /**** Set local positional variables for the first run ***/
                local_current_x = ntohl(return_message.avatar_turn.Pos[thread_id].x);
                local_current_y = ntohl(return_message.avatar_turn.Pos[thread_id].y);
                last_move_set_last_ID(last_thread_success_move, thread_id);
                last_move_set_last_success_dir(last_thread_success_move, M_EAST);
                last_move_set_initial_x(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].x)); // Initiates last successful move holder
                last_move_set_initial_y(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].y));
                last_move_set_x_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].x) + 1); // move east
                last_move_set_y_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].y));
            }

            // Temporary storage for the outcome of the move, to pass to the logging section. Values defined by const ints in header section of this file
            previous_move_code = -1;

            // Implicitly, the positive-branch of this if-statement executes only for a single thread (because on iteration zero, it is only one thread's turn)
            if (iteration_count == 0)
            {

                // For each of the avatars, create an avatar, set its position based on the return message, and save it to the avatar_array
                for (int i = 0; i < thread_initial_info_get_num_avatars(thread_info); i++)
                {
                    avatar_t *avatar = avatar_new(i);
                    int current_x = ntohl(return_message.avatar_turn.Pos[i].x);
                    int current_y = ntohl(return_message.avatar_turn.Pos[i].y);
                    position_t *position = position_new();
                    position_setX(position, current_x);
                    position_setY(position, current_y);
                    avatar_setPosition(avatar, position);
                    position_delete(position);
                    avatar_array_add(avatar_array, avatar);
                
                }

            }

            // Else, since it is not the first iteration ...
            else
            {

                // Update the position of the prior avatar in the avatar array
                int last_id = last_move_get_last_ID(SOT_last_move_global);
                int current_x = ntohl(return_message.avatar_turn.Pos[last_id].x);
                int current_y = ntohl(return_message.avatar_turn.Pos[last_id].y);
                int attempted_x = last_move_get_x_attempt(SOT_last_move_global);
                int attempted_y = last_move_get_y_attempt(SOT_last_move_global);
                int initial_x = last_move_get_initial_x(SOT_last_move_global);
                int initial_y = last_move_get_initial_y(SOT_last_move_global);


                // If the prior move was successful ...
                if (current_x == attempted_x && current_y == attempted_y)
                {

                    // Record the previous_move_code for passing to logging
                    previous_move_code = prev_move_path;

                    // In the avatar array, update the position of the prior avatar
                    position_setX(avatar_getPosition(avatar_array[last_id]), current_x);
                    position_setY(avatar_getPosition(avatar_array[last_id]), current_y);

                    // Wall-filler: this fills in traps identified by the previous thread.

                    // For the cell that the prior avatar left, check the relationships to the neighboring cells and see how many are walled
                    int wall_count = 0;
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x + 1, initial_y, initial_x, initial_y)){wall_count++;}
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x, initial_y + 1, initial_x, initial_y)){wall_count++;}
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x - 1, initial_y, initial_x, initial_y)){wall_count++;}
                    if (map_isWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x, initial_y - 1, initial_x, initial_y)){wall_count++;}
                    
                    // If the number of walls found is 3 or greater, a trap is found
                    if (wall_count > 2) {
                        if (avatar_array_find_avatar(thread_initial_info_get_SOT_avatar_array(thread_info), 
                        thread_initial_info_get_num_avatars(thread_info), initial_x, initial_y) == -1) {
                            
                            // Set the wall
                            map_setWallXY(thread_initial_info_get_SOT_shared_map(thread_info), initial_x, initial_y, current_x, current_y);

                            // Update the previous_move_code for passing to logging
                            previous_move_code = prev_move_path_fill;
                       }
                    }

                }

                // If the prior move was not successful ...
                else if (current_x == initial_x && current_y == initial_y)
                {
                    // Prior avatar did not move, so the path it tried to explore is a wall. Set it.
                    map_setWallXY(thread_initial_info_get_SOT_shared_map(thread_info), attempted_x, attempted_y, initial_x, initial_y);

                    // Set code for logging
                    previous_move_code = prev_move_wall;

                }

                // Update last move for individual avatar (if succesful)
                local_current_x = ntohl(return_message.avatar_turn.Pos[thread_id].x);
                local_current_y = ntohl(return_message.avatar_turn.Pos[thread_id].y);
                local_attempted_x = last_move_get_x_attempt(last_thread_success_move);
                local_attempted_y = last_move_get_y_attempt(last_thread_success_move);

                if (local_current_x == local_attempted_x && local_current_y == local_attempted_y)
                {
                    // successful move, update last succesful direction
                    int last_dir = find_last_direction(last_thread_success_move);
                    //printf("Thead ID %d, Calculated Last Direction:%d\n", thread_id, last_dir);
                    last_move_set_last_success_dir(last_thread_success_move, last_dir);
                }
            }


            /*** 3. Run decision algorithm to determine next move ***/
            // Any changes to shared data structures which are made by the decision algorithm must be clearly documented
            // Again, this is to help us ensure data structure consistency across threads and avoid horrible errors (e.g. race conditions, deadlock)

            int attempted_move = move_for_rhr(last_thread_success_move, thread_initial_info_get_SOT_shared_map(thread_info), local_current_x, local_current_y);


            /*** 4. Create a move message to move the avatar in the direction returned by the decision algorithm. ***/
            AM_Message move_message;
            memset(&move_message, 0, sizeof(move_message));
            move_message.type = htonl(AM_AVATAR_MOVE);
            move_message.avatar_move.AvatarId = htonl(thread_id);
            // Calculate next move for the avatar
            move_message.avatar_move.Direction = htonl(attempted_move);

            /*** 5. Update the SOT_last_move_global struct for the move requested in this iteration ***/
            last_move_set_last_ID(SOT_last_move_global, thread_id);

            last_move_set_initial_x(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].x));
            last_move_set_initial_y(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].y));

            last_move_set_initial_x(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].x));
            last_move_set_initial_y(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].y));

            // Based on direction, set the attempted x and y for both the global-scope (Scope 1) and the thread-scope (Scope 2) last-move structs
            if (attempted_move == M_EAST)
            {

                last_move_set_x_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].x) + 1);
                last_move_set_y_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].y));

                last_move_set_x_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].x) + 1);
                last_move_set_y_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].y));

            }
            else if (attempted_move == M_SOUTH)
            {

                last_move_set_x_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].x));
                last_move_set_y_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].y) + 1);

                last_move_set_x_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].x));
                last_move_set_y_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].y) + 1);

            }
            else if (attempted_move == M_WEST)
            {

                last_move_set_x_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].x) - 1);
                last_move_set_y_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].y));

                last_move_set_x_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].x) - 1);
                last_move_set_y_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].y));

            }
            else if (attempted_move == M_NORTH)
            {

                last_move_set_x_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].x));
                last_move_set_y_attempt(SOT_last_move_global, ntohl(return_message.avatar_turn.Pos[thread_id].y) - 1);

                last_move_set_x_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].x));
                last_move_set_y_attempt(last_thread_success_move, ntohl(return_message.avatar_turn.Pos[thread_id].y) - 1);

            }

            /*** 5. Calls to PRINT and LOGGING ***/
            // All calls to print and logging happen in this section (except for AM_MAZE_SOLVED). Any decisions made before which affect logging are constructed
            // and passed to here for logging.
            
            // Clear the screen between prints
            system("@cls||clear");

            // Call the simpleprint module
            print_map(
                thread_initial_info_get_MazeHeight(thread_info),
                thread_initial_info_get_MazeWidth(thread_info),
                thread_initial_info_get_SOT_avatar_array(thread_info),
                thread_initial_info_get_num_avatars(thread_info),
                thread_initial_info_get_SOT_shared_map(thread_info));

            // Logging

            // Open the file
            FILE *fp;
            fp = fopen(thread_initial_info_get_log_file_name(thread_info), "a");
            
            // If an error is encountered opening the file, skip this log cycle; otherwise, log to file
            if (fp != NULL) 
            {
                // Log the iteration number and turn ID:
                fprintf(fp, "\n\nIteration: %d. It is avatar #%d's turn. Current positions by avatar: \n", iteration_count, thread_id);

                // Log the current position of each avatar:
                for (int avatar_idx = 0; avatar_idx < thread_initial_info_get_num_avatars(thread_info); avatar_idx++)
                {
                    int log_current_x = ntohl(return_message.avatar_turn.Pos[avatar_idx].x);
                    int log_current_y = ntohl(return_message.avatar_turn.Pos[avatar_idx].y);
                    fprintf(fp, "\tAvatar ID: %d X: %d Y: %d \n", avatar_idx, log_current_x, log_current_y);
                }

                // If it is not the first turn, then log the results of the prior turn, 
                if (iteration_count != 0 ) { 
                    
                    fprintf(fp, "\tThe result of the previous turn is: ");
                    if (previous_move_code == prev_move_wall) 
                    {
                        fprintf(fp, "Wall found.");
                    } 
                    else if (previous_move_code == prev_move_path) 
                    {
                        fprintf(fp, "Move successful.");
                    } 
                    else if (previous_move_code == prev_move_path_fill) 
                    {
                        fprintf(fp, "Move successful and wall filled behind.");
                    }
                } else {
                    fprintf(fp, "\tFirst move, so no prior move to report.");
                }

                // Log the requested move
                fprintf(fp, "\n\tAvatar #%d is now requesting to move ", thread_id);
                if (attempted_move == M_EAST)
                {
                    fprintf(fp, "east.\n");
                }
                else if (attempted_move == M_SOUTH)
                {
                    fprintf(fp, "south.\n");
                }
                else if (attempted_move == M_WEST)
                {
                    fprintf(fp, "west.\n");
                }
                else if (attempted_move == M_NORTH)
                {
                    fprintf(fp, "north.\n");
                }

                // Close the log file
                fclose(fp);
            }


            /*** 6. Write message out to the server ***/

            // Conduct the write and throw an error if any problem in writing
            int bytesWritten;
            if ((bytesWritten = write(sock, &move_message, sizeof(move_message))) < 0)
            {
                fprintf(stderr, "\tError sending move to server\n");
                exit(7);
            }

            /*** 7. Handler for this iteration ends here ***/

        }

        /*** 6. Exit lock ***/
        pthread_mutex_unlock(&mutexReadAndWrite);

        // Update the iteration count
        iteration_count++;

        // DEBUG Setting: If the debug setting is on, and the number of cycles allowed has been reached, exit the thread
        if (DEBUG_SWITCH_ITR == 1)
        {
            if (iteration_count == END_RUN_ITR)
            {
                break;
            }
        }

        /*** While loop iteration ends here ***/
    }

    // If execution exits the while loop, then free memory and exit.
    thread_initial_info_delete(thread_info);
    last_move_delete(last_thread_success_move);
    return NULL; 
    pthread_exit(0);
}

/**************** move_for_rhr ****************/
/* Suggests next movement using right-hand method 
 * Given: previous location, current location, previous move 
 * Returns: uint32_t move type as defined in amazing.h 
 * 1. Checks if there's a difference between current position and last position
 *    i. if there's a difference, there's a wall between previous location and previous move 
 * 2. Suggests to go right unless previous move was right and no movement
 * 3. Suggests to go in another direction  
 */
uint32_t move_for_rhr(last_move_t *last_move, map_t *amap, int current_x, int current_y)
{
    
    // Check if avatar is at meeting point
    if (avatar_at_point(current_x, current_y, map_getMazeWidth(amap))) {
        return M_EAST; // stays at top right corner 
    }

    // Depending on the direction of the last successful move, decide the next move in preference order according to the right hand rule
    // for each direction considered, check whether there is a wall in that direction, and if not, return that as the direction to move next

    // If the direction of the last successful move was west...
    if (last_move_get_last_success_dir(last_move) == M_WEST)
    {
        if (!map_isWallXY(amap, current_x, current_y, current_x, current_y - 1))
        {
            return M_NORTH;
        }
        else if (!map_isWallXY(amap, current_x, current_y, current_x - 1, current_y)) 
        {
            return M_WEST;
        }
        else if (!map_isWallXY(amap, current_x, current_y, current_x, current_y + 1))
        {
            return M_SOUTH;
        }
        else
        {
            return M_EAST;
        }
    }

    // If the direction of the last successful move was north...
    else if (last_move_get_last_success_dir(last_move) == M_NORTH)
    {
        if (!map_isWallXY(amap, current_x, current_y, current_x + 1, current_y))
        {
            return M_EAST;
        }
        else if (!map_isWallXY(amap, current_x, current_y, current_x, current_y - 1))
        {
            return M_NORTH;
        }

        else if (!map_isWallXY(amap, current_x, current_y, current_x - 1, current_y))
        {
            return M_WEST;
        }
        else
        {
            return M_SOUTH;
        }
    }

    // If the direction of the last successful move was east...
    else if (last_move_get_last_success_dir(last_move) == M_EAST)
    {
        if (!map_isWallXY(amap, current_x, current_y, current_x, current_y + 1))
        {
            return M_SOUTH;
        }

        else if (!map_isWallXY(amap, current_x, current_y, current_x + 1, current_y))
        {
            return M_EAST;
        }

        else if (!map_isWallXY(amap, current_x, current_y, current_x, current_y - 1))
        {
            return M_NORTH;
        }
        else
        {
            return M_WEST;
        }
    }

    // If the direction of the last successful move was south...
    else if (last_move_get_last_success_dir(last_move) == M_SOUTH)
    {                                                                            
        if (!map_isWallXY(amap, current_x, current_y, current_x - 1, current_y))
        {
            return M_WEST;
        }
        else if (!map_isWallXY(amap, current_x, current_y, current_x, current_y + 1)) 
        {
            return M_SOUTH;
        }
        else if (!map_isWallXY(amap, current_x, current_y, current_x + 1, current_y))
        {
            return M_EAST;
        }
        else
        {
            return M_NORTH;
        }
    }

    // Execution should not reach this point. Return -1 indicating error
    return (uint32_t)(-1);
}


/**************** find_last_direction ****************/
/* Given a last move struct, the function returns the last direction expressed by the move as an integer based off of:
 *  West - 0
 *  North - 1
 *  South - 2
 *  East - 3
 */
uint32_t find_last_direction(last_move_t *last_move)
{
    if (last_move_get_initial_x(last_move) < last_move_get_x_attempt(last_move))
    {
        return M_EAST;
    }
    else if (last_move_get_initial_x(last_move) > last_move_get_x_attempt(last_move))
    {
        return M_WEST;
    }
    else if (last_move_get_initial_y(last_move) > last_move_get_y_attempt(last_move))
    {
        return M_NORTH;
    }
    else if (last_move_get_initial_y(last_move) < last_move_get_y_attempt(last_move))
    {
        return M_SOUTH;
    }

    // Execution should not reach this point. Return -1 indicating error
    return (uint32_t)(-1);
    
}

/**************** avatar_at_point ****************/
/* Finds if avatar is at a designated meeting point 
 * @param: int current_x: avatar's current x position
 * @param: int current_y: avatar's current y position
 * @param: int maze_width: current maze's width 
 * @return: boolean if avatar is at designated meeting point (top right corner) 
 * Check if avatar is at: 
 * Y: 0
 * X: MAZE_WIDTH - 1 
 */
bool avatar_at_point(int current_x, int current_y, int maze_width) {
    if ((current_x == maze_width - 1) && (current_y == 0)) {
        return true; 
    }
    return false; 
}
