#!/bin/bash

set -eu

mkdir -p build/
cd build/
# command line arguments
if [ $# -ne 1 ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
    cmake --build . -j 16
else
    if [ $1 = "coverage" ]; then 
        cmake -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
        cmake --build . -j 16
    fi
fi


