/* ========================================================================== */
/* File: AMStartup.c
 *
 * Project name:    CS50 Amazing Project
 * Team name:       PiedPiper
 * Authors:         Andrw Yang, Maria Roodnitsky, Siddharth Agrawal, Alexander Hirsch
 * Component name:  AMStartup
 * 
 * 
 * Date Created:    February 25, 2020
 * Last Updated:    March 8, 2020
 * 
 * Description:     AMStartup is the main driver for Team PiedPiper's Maze Solver.
 *                  It is called by the user following the usage below. It then
 *                  validates user input, establishes contact with the server identity
 *                  provided by the user (port number assigned in amazing.h). It then
 *                  calls the AMClient module to begin game solving.
 * 
 * Usage:           ./AMStartup [Number of avatars] [Difficulty level] [Hostname]
 *                  [Number of avatars] must be between 1 and 10 inclusive
 *                  [Difficulty level] must be between 0 and 9 inclusive
 *                  [Hostname] must be a valid server IP address
 */
/* ========================================================================== */

// Import C Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

// Import project specific libraries
#include "libs/amazing.h"
#include "libs/AMlib.h"
#include "libs/AMClient.h"

/**************** local functions ****************/
int AMStartup_Valid_Numeric_Inputs(const int argc, const char *argv[]);
AM_Message *AMStartup_Create_AM_INIT(class_variables_t *cv);
int AMStartup_Create_Logfile(class_variables_t *cv);

/**************** main ****************/
int main(const int argc, const char *argv[])
{

    // Complete input validation
    int return_value;
    if ((return_value = AMStartup_Valid_Numeric_Inputs(argc, argv)) != 0) {
        exit(return_value);
    }

    // Allocate and initialize class_variables struct
    class_variables_t *variables_holder = class_variables_new(atoi(argv[1]), atoi(argv[2]), argv[3]);
    if (variables_holder == NULL)
    {
        fprintf(stderr, "ERROR: 3: error allocating memory for variables_holder. Exiting. \n");
        exit(3);
    }

    // Create socket connection
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        class_variables_delete(variables_holder);
        fprintf(stderr, "ERROR: 4: Error opening socket. Exiting. \n");
        exit(4);
    }

    // Create socket address struct
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(AM_SERVER_PORT));

    // Create host struct, thereby validating given hostname
    struct hostent *host = gethostbyname(class_variables_get_hostname(variables_holder));
    if (host == NULL)
    {
        class_variables_delete(variables_holder);
        fprintf(stderr, "ERROR: 5: Unknown host. Exiting. \n");
        exit(5);
    }

    memcpy(&servaddr.sin_addr, host->h_addr_list[0], host->h_length);

    // Attempt to establish contact with server. If connect fails, exit.
    if ((connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
    {
        class_variables_delete(variables_holder);
        fprintf(stderr, "ERROR: 6: Error connecting to server. Exiting. \n");
        exit(6);
    }

    // Create AM_INIT message per user-set parameters
    AM_Message *init_message;
    init_message = AMStartup_Create_AM_INIT(variables_holder);
    if (init_message == NULL) {
        fprintf(stderr, "ERROR: 7: Error creating AM_INIT message struct. Exiting. \n");
        exit(7);
    }

    // Write the AM_INIT message to the server
    if ((write(sock, init_message, sizeof(AM_Message))) < 0)
    {
        class_variables_delete(variables_holder);
        free(init_message);
        fprintf(stderr, "ERROR: 8: Error writing to server. Exiting. \n");
        exit(8);
    }
    
    // Free the AM_INIT message struct
    free(init_message);

    // Attempt to receive an AM_INIT_OK message from server
    AM_Message *return_message;
    return_message = calloc(1, sizeof(AM_Message));
    if (read(sock, return_message, sizeof(AM_Message)) < 0)
    {
        free(return_message);
        class_variables_delete(variables_holder);
        fprintf(stderr, "ERROR: 9: Error reading message from server. Exiting. \n");
        exit(9);
    }
    
    // If the received message type is not AM_INIT_OK, then throw an error and exit
    if (ntohl((uint32_t)return_message->type) != AM_INIT_OK)
    {
        fprintf(stderr, "ERROR: 10: Initialization failed. Received a message other than AM_INIT_OK from the server. Exiting. \n");
        free(return_message);
        class_variables_delete(variables_holder);
        exit(10);
    }

    // Save maze parameters received from the server as part of AM_INIT_OK message, into class_variables structure
    class_variables_set_MazeHeight(variables_holder, ntohl(return_message->init_ok.MazeHeight));
    class_variables_set_MazeWidth(variables_holder, ntohl(return_message->init_ok.MazeWidth));
    class_variables_set_mazePort(variables_holder, ntohl(return_message->init_ok.MazePort));
    
    // Now that the parameters are saved, free the return_message
    free(return_message);

    // Create and write header to a logfile for this run
    int logfile_return_value;
    if ((logfile_return_value = AMStartup_Create_Logfile(variables_holder)) != 0) 
    {
        exit(logfile_return_value);
    }

    // Maze setup is now complete
    // Call the AMClient module, which is the main driver of maze solving, using its function client_start()
    // See AMClient.[ch] for details
    if (!client_start(variables_holder)) 
    {
        fprintf(stderr, "ERROR: 41: Failure within AMClient module's Client_start. Exiting. \n");
        exit(41);
    };

    // Clean up structures allocated in this module
    class_variables_delete(variables_holder);

    // Program complete. Exit 0 on success.
    exit(0);

}

/******** Helper functions ********/

/******** AMStartup_Valid_Numeric_Inputs ********/
/* AMStartup_Valid_Numeric_Inputs validates the inputs provided by the user.
 * Returns:
 * - 0 if all inputs pass requirements
 * - non-zero otherwise
 */
int AMStartup_Valid_Numeric_Inputs(const int argc, const char *argv[])
{
    // Validate number of arguments
    if (argc != 4)
    {
        fprintf(stderr, "ERROR: 11: Incorrect # of arguments entered. Must be four arguments (including file call).\n");
        return 11;
    }
    
    // Validate that arg1 [num_avatars] is numeric 
    int argv1len = strlen(argv[1]);
    for (int i = 0; i < argv1len; i++) {
        if (!isdigit(argv[1][i])) {
            fprintf(stderr, "ERROR: 12: [num_avatars] contains non-numeric input. Exiting. \n");
            return 12;        
        } 
    }

    // Validate that arg1 [num_avatars] is within bounds (1-10 inclusive)
    if ((atoi(argv[1]) < 1) || (atoi(argv[1]) > 10)) {
        fprintf(stderr, "ERROR: 13: [num_avatars] out of bounds. Bounds are 1-10 inclusive.\n");
        return 13;
    }

    // Validate that arg2 [difficulty_level] is numeric 
    int argv2len = strlen(argv[2]);
    for (int i = 0; i < argv2len; i++) {
        if (!isdigit(argv[2][i])) {
            fprintf(stderr, "ERROR: 14: [difficulty_level] contains non-numeric input. Exiting. \n");
            return 14;        
        } 
    }

    // Validate that arg2 [difficulty_level] is within bounds (0-9 inclusive)
    if ((atoi(argv[2]) < 0) || (atoi(argv[2]) > 9)) {
        fprintf(stderr, "ERROR: 15: [difficulty_level] out of bounds. Bounds are 0-9 inclusive.\n");
        return 15;
    }

    // Return zero if all input tests pass
    return 0;
}


/******** AMStartup_Create_AM_INIT ********/
/* AMStartup_Create_AM_INIT creates an AM_INIT message struct
 * with values corresponding to the user's given parameters.
 * Returns:
 * - AM_Message for AM_INIT
 * - NULL if any error
 */
AM_Message *AMStartup_Create_AM_INIT(class_variables_t *cv)
{
    // Allocate space for the message
    AM_Message *init_message;
    init_message = calloc(1, sizeof(AM_Message));
    if (init_message == NULL)
    {
        fprintf(stderr, "ERROR: Error allocating memory for init_message\n");
        return NULL;
    }
    
    // Set the values of the message
    init_message->type = htonl(AM_INIT);
    init_message->init.nAvatars = htonl((class_variables_get_num_avatars(cv)));
    init_message->init.Difficulty = htonl((class_variables_get_difficulty(cv)));

    // Return the finished message struct
    return init_message;
}

/******** AMStartup_Create_logfile ********/
/* AMStartup_Create_logfile creates and writes the header to a log file
 * for the run. Saves the logfile name to the class_variables struct provided
 * by the caller.
 * Memory: allocates memory for a file name and saves it into class_variables
 * struct provided by the caller. Caller is responsible for later calling
 * class_variables_delete on this struct, which will delete the logfile name
 * as well as the remainder of the data stored in this class_variables struct.
 * Returns:
 * - zero if successful
 * - non-zero if any error
 */
int AMStartup_Create_Logfile(class_variables_t *cv)
{
    // Get user name
    char *username = getenv("USER");

    // Convert the [num_avatars] and [difficulty_level] variables into strings
    char numAv[20];
    sprintf(numAv, "%d", class_variables_get_num_avatars(cv));
    char difficulty[20];
    sprintf(difficulty, "%d", class_variables_get_difficulty(cv));

    // Compose the file name by allocating memory and then writing the name
    char *filename = malloc(strlen("Amazing_") + strlen(username) + strlen("_") + strlen(numAv)
         + strlen("_") + strlen(difficulty) + strlen(".log") + 1);
    snprintf(filename, strlen("Amazing_") + strlen(username) + strlen("_") + strlen(numAv) 
        + strlen("_") + strlen(difficulty) + strlen(".log") + 1, 
        "%s%s%s%s%s%s%s", "Amazing_", username, "_", numAv, "_", difficulty, ".log");
    
    // Attempt to open the file for writing
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: 31: Opening the intended logfile name in write mode failed. The desired filename and path may be unwritable or located in an unwritable directory. Exiting. \n");
        return 31;
    }

    // Write the username to the file header
    fprintf(fp, "%s, ", username);

    // Write the mazePort to the file header
    char port[20];
    sprintf(port, "%d", class_variables_get_mazePort(cv));
    fprintf(fp, "%s, ", port);

    // Write the time to the file header
    time_t currtime;
    time(&currtime);
    char *time_string = ctime(&currtime);
    fprintf(fp, "%s", time_string);

    // Close the logfile
    fclose(fp);

    // Save the pointer to the logfile name into the class_variables struct. 
    class_variables_set_log_file_name(cv, filename);

    // Return 0 for successful completion
    return 0;
}