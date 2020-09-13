## TESTING.md

This file describes the testing approach for the Maze Solver, and it provides instructions on how to run the tests.

### Overview

Testing was conducted in three parts:

1. Full integration testing was performed across combinations of num_avatars and difficulty_level. 

2. Parameter testing was performed for the inputs to the program, to verify input validation.

3. Unit testing was conducted for several modules, map and print, which can be tested independently.

Handling of error messages from the server was tested on an ad-hoc basis as they were received (e.g. AM_INIT_FAILED, AM_DISK_SERVER_QUOTA).

All tests produce expected output and run without memory leaks or errors.

Instructions for how to view and run each set of results is listed below. The file paths and commands listed are expressed such that you can enter them from the repository's parent directory (the directory which hosts this file).

### 1. Full Integration Testing 

We wrote a script to test most combinations of avatars and difficulties

The script can be run at: 

    bash testscripts/test_every_maze.sh

#### File Format: 

    testoutputs/Amazing_[USER]_[NUMBER OF AVATARS]_[DIFFICULTY].log

 * USER: User running the program
 * NUMBER OF AVATARS: Number of avatars in the maze 
 * DIFFICULTY: Difficulty of the maze 

#### Accessing Results: 

Results of the test runs (because log files are long) can be viewed using: 

    tail -n 3 testoutputs/Amazing_dru_{0..9}_{0..9}.log   

#### Grading Criteria Highlights: 

* Solved mazes of difficulty up to 3 with 3 Avatars, basic ASCII UI

    tail -n 3 testoutputs/Amazing_dru_3_{0..3}.log 

* Solved mazes of difficulty up to 5 with 4 or more Avatars

    tail -n 3 testoutputs/Amazing_dru_{4..9}_5.log 

* Solved mazes of difficulty 7+ with 4 or more Avatars

    tail -n 3 testoutputs/Amazing_dru_7_7.log


### 2. `AMStartup` parameter initialization:

The parameters initialization test provides a variety of cases to verify correct handling of invalid input  

To run: `./testscripts/testing_parameters.sh`

The output is stored at: `/testoutputs/testing_parameters.out`

### 3. Unit testing for 'map' and 'print'

#### Map

To run: `mygcc maptest.c ../libs/map.c ../libs/AMlib_avatar.c -o maptest` followed by `./maptest`
The output is stored at: `/testoutputs/maptest.out`

#### Print

To run: `mygcc printtest.c ../libs/simpleprint.c ../libs/map.c ../libs/AMlib_avatar.c -o printtest` followed by `./printtest`
The output is stored at: `/testoutputs/maptest.out`
