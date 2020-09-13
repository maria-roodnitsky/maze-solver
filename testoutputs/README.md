# testoutputs

## README.md

This folder contains the logfiles/outputs of our testing. Please refer to TESTING.md in the parent directory for more information on our test process.

*NOTICE*: Some testing combinations were not succesful due to `AM_SERVER_DISK_QUOTA`. Those tests have been removed. 

## File Format: 

    Amazing_[USER]_[NUMBER OF AVATARS]_[DIFFICULTY].log

 * USER: User running the program
 * NUMBER OF AVATARS: Number of avatars in the maze 
 * DIFFICULTY: Difficulty of the maze 

## Accessing Results: 

Results of the test runs (because log files are long) can be viewed using: 

    tail -n 3 Amazing_dru_{0..9}_{0..9}.log   

## Grading Criteria Highlights: 

* Solved mazes of difficulty up to 3 with 3 Avatars, basic ASCII UI

    tail -n 3 Amazing_dru_3_{0..3}.log 

* Solved mazes of difficulty up to 5 with 4 or more Avatars

    tail -n 3 Amazing_dru_{4..9}_5.log 

* Solved mazes of difficulty 7+ with 4 or more Avatars

    tail -n 3 Amazing_dru_7_7.log
