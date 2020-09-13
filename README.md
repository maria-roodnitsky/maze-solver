# project_maze_challenge
# CS50 Winter 2020, Final Project

## Team name: Pied Piper
## Team Members: Andrw Yang, Siddharth Agrawal, Maria Roodnitsky, and Alexander Hirsch 

GitHub usernames: f003kgm (Maria), ssagrawal01 (Siddharth), zanderh3 (Alexander), druyang (Andrw) 

Final presentation can be seen here: 
https://docs.google.com/presentation/d/19MbTdJ4Y4scTl81Q2VhmfVYgijOhMfeTtsNV6rx0ueI/edit?usp=sharing


### Compiling and Building 

To build the main file, you can run the following command from this directory (it will make a library inside the lib folder, and then make an executable)
`cd libs; make clean; make ; cd ..; make clean; make`

Our compilation requires the following compiler arguments and dependencies: 
* `c11` - C Programming Language (Standard)
* `-lpthread` - links `pthread` library for multithreading (utilized by `AMClient.c` as each avatar is a thread)
* `-D_GNU_SOURCE`- enables GNU extensions as defined by POSIX.1, this enables our program and compiler to utilize the most efficient `strdup()` for string duplication

### Running

To run, you can run the following command from this directory:
`./AMStartup [num_avatars] [difficulty_level] flume.cs.dartmouth.edu`

### Testing

How to run testing is summarized in TESTING.md. Test scripts are located in the folder `testscripts/` and test outputs are located in the folder `testoutputs/`.

### Directory Contents
* AMStartup.c
* Makefile
* TESTING.md
* DESIGN.md
* testscripts/
* testoutputs/


### Implementation Strategy

Implementation strategy for the Maze Solving algorithm is discussed in detail in the `libs/README.md`.

### Acknowledgements

For our gitignores, we leveraged the standard C .gitignore proivided by GitHub as part of creating our repository.

For clearing the screen, we learned from the following resource:
https://stackoverflow.com/questions/2347770/how-do-you-clear-the-console-screen-in-c

