#!/bin/bash

arg=$1

if [ $# -eq 1 ]; then
    if [ $arg = "clean" ]; then
        cd ../src
        rm -f cmake_install.cmake CMakeCache.txt
        rm -rf CMakeFiles
        rm -f child main
        rm -f Makefile
        exit 0
    fi
fi

cd ../src
cmake CMakeLists.txt
make

cd ../test
array=(`find test*`)
for TEST in $(ls test*)
do
    cd ../test
    echo
    tput setaf 5; echo $TEST; tput setaf 7;
    cat $TEST
    cp $TEST ../src
    cd ../src
    echo
    tput setaf 6; echo "Answer: "; tput setaf 7;
    ./main $TEST
    rm $TEST
    echo
done

