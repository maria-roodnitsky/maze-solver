## Maze Solver

### README.md for the libs/ directory

The libs/ directory includes the following modules:
* AMClient:     Main driver for the threads and maze solving
* AMLib:        Contains the class_variables, thread_initial_info, and last_move structs and their export functions
* AMLib_avatar: Contains the position, avatar, and avatar_array structs and their export functions
* map:          Provides a map for the threads to share
* simpleprint:  Prints the current state of game play in an ASCII display

These files are compiled into maze_lib.a

### Implementation Strategy

Our algorithm is implemented using the right-hand-rule (RHR) strategy as well as trap-filling. This component is implemented through a suite of functions within AMClient.c.

The right-hand-rule functions by having each player do the following for each move:
* If possible, move right
* If not possible, move forward
* If not possible, move left
* If not possible, move backward

It is a suitable choice for this problem because the maze is "perfect," meaning it has no cycles and only one connected component. Therefore, it is a tree. The algorithm can essentially be thought of as Depth-First-Search on a tree where the player's current position is root (i.e. the player traverses each branch of the tree in full before going on to the next), and the wall-filling action can be thought of as "pruning" the tree.

On each turn, the players go through the following process:
* Check whether the message received from the server is a special message, such as AM_SOLVED or an error message.
* If not, check whether the message is an AM_AVATAR_TURN message and whether it is that avatar's turn.

From here, only the avatar whose turn it is (the current avatar) takes any further action. The current avatar first updates the source-of-truth structs (map, avatar_array) based on information learned from the AM_AVATAR_TURN message and a last_move struct which holds information about what the previous player attempted to do. The current player interprets the results of the previous player's actions (wall found? open path found? trap found to be filled?), resulting in an up-to-date source of truth.

Next, the current avatar implements the decision-algorithm to decide what its next move should be. The next move is based on the right-hand-algorithm discussed above, and it first checks the map to see which connections to other cells are already known to be walls. In order to do this, each avatar maintains a notion of what its "cardinal direction" is: that is, what direction is it facing? This is implemented using last_thread_success_move, which exists individually for each avatar and is updated each time an avatar makes a move successfully (which may change its direction).

The below section gives an overview of the structs used by scope:

### Important structures by Scope

#### Scope 1: Shared among all threads
*Variable name and supporting struct typedef*
* class_variables             (class_variables)
* SOT_shared_map_global       (map)
* SOT_avatar_array            (avatar_array)
* SOT_last_move_global        (last_move)

#### Scope 2: Unique to each thread and persistent over life of thread
* thread_info                 (thread_initial_info)   
* last_thread_success_move    (last_move)   

Thread_info contains pointers to elements of the Scope 1 structs, and is generated and passed in at the creation of each thread, so that the thread can reference the Scope 1 structs.