#!/bin/bash
#
# testing_parameters.sh
#
# Authors:      Maria Roodnitsky, Siddharth Agrawal, Andrw Yang, Alxander Hirsch
# Date:         March 2020
#
# Usage:        ./testing_parameters.sh
# 
# Description:  Tests parameter handling for AMStartup.c
#

# Zero arguments
valgrind ./AMStartup

# One argument
valgrind ./AMStartup 3

# Two arguments
valgrind ./AMStartup 3 2

# More than three arguments
valgrind ./AMStartup 3 2 flume.cs.dartmouth.edu toomanyarguments

# String when numerical expected
valgrind ./AMStartup imastring 2 flume.cs.dartmouth.edu
valgrind ./AMStartup 3 metoo flume.cs.dartmouth.edu
valgrind ./AMStartup h i flume.cs.dartmouth.edu
valgrind ./AMStartup 3 i flume.cs.dartmouth.edu

# Numerical input boundary conditions
valgrind ./AMStartup 1 2 flume.cs.dartmouth.edu
valgrind ./AMStartup 11 2 flume.cs.dartmouth.edu
valgrind ./AMStartup 2 -1 flume.cs.dartmouth.edu
valgrind ./AMStartup 2 10 flume.cs.dartmouth.edu

# Hostname not conforming to assignment
valgrind ./AMStartup 2 10 fakemoose.cs.dartmouth.edu

