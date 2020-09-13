# Makefile for 'AMStartup' module
# Builds and compiles AMStartup given a maze_lib.a file 
# Team PiedPiper, Andrw Yang Febuary 2020 

L = ../libs 
LLIBS = libs/maze_lib.a
PROG = AMStartup 
OBJS = AMStartup.o

# Our compiler and its flags
CC = gcc
CFLAGS = -Wall -pedantic --std=c11 -D_GNU_SOURCE -ggdb $(FLAGS) -lpthread -I$(L) 
MAKE = make 

# 'phony' targets are helpful but do not create any file by that name
.PHONY: clean all test 

all: AMStartup 

# make the program based on its object files 
AMStartup: AMStartup.o 
	$(CC) $(CFLAGS) AMStartup.o $(LLIBS) -o AMStartup

# object files 
AMStartup.o: libs/amazing.h libs/AMClient.h libs/AMlib_avatar.h libs/AMlib.h libs/map.h

# to clean up all derived files
clean: 
	rm -f *~
	rm -f $(LIB)
	rm -f *.o 
	rm -f $(PROG)

test: 
	./testing.sh