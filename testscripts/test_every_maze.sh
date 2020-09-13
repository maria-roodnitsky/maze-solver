#!/bin/bash
echo "Bash version ${BASH_VERSION}..."

for AVATAR in {0..9}
do 
    for DIFF in {0..9}
    do 
        echo "---------------------------------------------------------"
        echo "${AVATAR} avatars at ${DIFF} difficulty "
        ../AMStartup.app ${AVATAR} ${DIFF} flume.cs.dartmouth.edu | tail -100 
        echo "Output logs to designated location" 
        echo ""
    done 
done 