# Makefile for libraries, based off of libcs50 makefile
# Builds and compiles all these utility functions into a library.
#
#
# David Kotz, April 2016, July 2017, April 2019
# Updated by Xia Zhou, July 2016
# Updated by Temi Prioleau, January 2020
# Updated by by Team Pied Piper
# Andrw Yang, Febuary 2020 

# object files, and the target library
OBJS = AMClient.o AMlib.o AMlib_avatar.o map.o simpleprint.o
#map.o 
LIB = maze_lib.a

FLAGS = -lpthread

CFLAGS = -Wall -pedantic --std=c11 -D_GNU_SOURCE -ggdb -lpthread $(FLAGS)  -DVERBOSE
CC = gcc
MAKE = make

# Build the library by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
AMClient.o: AMClient.h AMlib.h
AMlib.o: AMlib.h amazing.h
amazing.o: amazing.h
map.o: map.h
AMlib_avatar.o: AMlib_avatar.h
simpleprint.o: simpleprint.h

.PHONY: clean sourcelist

# list all the sources and docs in this directory.
# (this rule is used only by the Professor in preparing the starter kit)
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o
