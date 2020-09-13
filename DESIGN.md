# Team PiedPiper

### Design Doc for The Maze Solver Project

#### User interface
Very little of the maze solver is ever directly interacted with by the user. The user calls the `AMStartup` executable from the command line -- providing the initialization parameters to determine who is solving the maze (the count of avatars), which maze is being solved (the difficulty of the maze; with 0 being a simple maze, 9 being a quite challenging one), and where the maze is stored (the hostname of the server). From there, other modules take over the maze solving component.

The `AMStartup` commandline usage is as follows:
./AMStartup [number_of_avatars] [difficulty_level] [host_name]

`number_of_avatars` must be an integer value between 1 and 10. 
`difficulty_level` must be an integer value between 0 and 9.
`hostname` must be a valid name of a host server. For our scope, this is *flume.cs.dartmouth.edu*

#### Inputs and Outputs
Inputs: The only inputs are from the command line in the `AMStartup`. 

Outputs: The maze solver produces two output components upon successful compeletion. It prints an ASCII representation of the maze to the standard output as each move is made, and it creates a log file and tracks these moves in a specified format. 

#### Functional decomposition into modules

`The Maze Solver` is supported by several modules:
1. *AMStartup* is the executable run by the user. First, it validates user inputs. It then pings the server to get parameters with which to initialize information about the maze (width, height, and server port number). It creates the log file and calls a function stored in `AMClient` to continue the game.
2. *AMClient* is the driver of game execution. It includes a "parent function" which creates `num_avatars` threads. Game execution proceeds turnwise. When it is a thread's turn, that thread updates the map, updates the log file, prints to screen, and calculates and executes its next move. When the threads receive the `MAZE_SOLVED` message from the server, the client and all the threads exit. 
3. *map* implements the map and exports a set of functions which other modules can use without reference to the map-internals.
4. *algorithm* on each thread's turn, this module executes the turn's decision-making about what move to make next
5. *print* enables printing of the current state of the map to screen

`The Maze Solver` is also supported by several library modules:

6. *amLib* is a library file of structs which are independently used by the other modules, including class_variables, thread_iniitial_info, and last_move.
7. *amLib_avatar* is a library file of structures which support notions of avatar, including position, avatar, and an array of avatars

#### Pseudocode for logic/algorithmic flow

##### AMStartup
1. Input validation
2. Send an initialization message AM_INIT to the server, containing `num_avatars` and `difficulty_level`
3. Receive AM_INIT_OK message, and parse it for the `mazeHeight`, `mazeWidth`, and `mazePort`
4. Create the log file
5. Call the AMClient

##### AMClient
1. Initialize global data structures (map, avatar_array, last_move, thread_initial_info)
2. Spawn `num_avatars` threads. Each thread represents one player.
3. Within each thread:
   1. Initialize thread-specific data structures (last successful move of thread, iteration count)
   1. Create socket
   3. Connect socket
   4. Send an AM_AVATAR_READY message to the server
   5. While the maze is not solved
      1. Receive a message from the server
      2. If the maze is solved or an error is received
         1. If the thread is thread 0, update the log file
         2. exit
      3. If an AM_AVATAR_TURN message is received, containing whose turn it is and the current positions of each player, and it is the thread's turn:
         1. Update the global map with the results of the last move
            1. If the previous turn's move was successful, update the connection between the cells with an open marker, and update the position of that avatar
            2. If the previous turn's move was unsuccessful, update the connection between the cells with a wall
            3. If the previous turn's move was out of a trap, fill the wall behind it
         2. Print the screen output
         3. Write the log file output
         4. Run the right-hand-rule algorithm to decide the next move, using the map status and the current direction of the avatar
         5. Send a move request to the server

##### map
1. Export functions to get and set the status of relationships between cells, and get and set the positions of avatars
2. Initialize a map with given parameters `mazeHeight` and `mazeWidth`.
3. Implement the map as an occupancy grid of dimensions (2*`MazeHeight`-1 x 2*`MazeWidth`-1), where even index rows and columns represent cells, and odd index rows and columns represent relationships between cells
4. When a call for updating an edge is received, update the occupancy grid position representing the relationship between those cells
5. When calls for getting positions are received, serve the request

##### algorithm
1. Implement the *wall following algorithm* coupled with *trap filling*, and stop motion of avatar if it reaches the upper-right-hand corner of the maze
   We learned about the wall following algorithm from the following source provided in class: http://www.astrolog.org/labyrnth/algrithm.htm
2. The wall following algorithm will proceed as follows for each thread:
   1. Update the direction of the avatar based on its last successful move
   2. Given the direction of the avatar and the known state of the global map:
      1. If possible to turn right, turn right
      2. If not possible, proceed ahead
      3. If not possible, proceed left
      4. If not possible, proceed in reverse direction (direction defined by prior motion)
3. If this avatar left from a trap, the next avatar to go will fill in the wall behind this avatar

##### print
1. For each cell in the maze
   1. Initialize strings to store the printed material for that cell
   2. For each possible connection to another cell, 
      1. If there exists a wall between this cell and the adjoining cell, or the adjoining cell isn't in the map:
         1. Add the appropriate walls to the print area
   3. If the cell is occupied
      1. Add the ID numbers of the avatar in the cell to the print area
2. Print each line in the maze
   1. For the strings corresponding to each line from each cell
      1. Append the lines together
      2. Print them out

#### Dataflow through modules

* `AMStartup` takes parameters from the user (`num_avatars`, `difficulty_level`, and `hostname`) and uses these to establish a connection with the server. Using the server connection, it gets a `port_number` and the dimensions of the maze, which it passes to `AMClient`.
* With those variables, `AMClient` creates and stores the map, an array of avatars, and a common holder for the last move shared among the threads. It then spawns `num_avatars` threads, and each thread leverages and modifies these variables on its turn only.
* During each turn, the thread whose turn it is updates the shared data structures such as the map, and then executes the algorithm to determine its next move. It sends that message to the server.
* During each turn, the map is piped into the print function for display to screen, and the status of the latest turn is piped into the logging function.
* The algorithm proceeds until the all players are in the same position and the maze is solved or the number of turns allowed has been exhausted.
* When the algorithm execution completes, the threads close and data structures are released.

#### Major data structures
1. *class_variables* - stores variables passed in by the user and returned by AM_INIT_OK
2. *SOT_shared_map* - global map source-of-truth
3. *SOT_avatar_array* - global avatar array source-of-truth
4. *SOT_last_move_global* - global last move source-of-truth
5. *thread_initial_info* - stores refererences to shared data structures, and is passed into each thread when they are created
6. *map* - stores an occupancy grid representing the map (method described in *Functional Decomposition of modules* section)
7. *position* - stores a coordinate pair (x and y) representing a cell in the map
8. *avatar* - stores the ID number of an avatar and its position
9. *avatar_array* - stores references to each avatar

#### Testing plan

Testing will proceed at integration, parameter, and unit levels.

Integration testing will begin with a level 0 maze and 2 avatars and progress in increasing difficulty level and avatar count up to level 9 and 10 avatars respectively.

Parameter testing will verify that invalid user inputs are properly handled.

Unit testing will be conducted on major modules (e.g. map, print) using test driver files. These tests will check for correctness of results in response to the input stream (e.g. add a wall and check whether it is correctly stored). Boundary conditions will also be tested.

