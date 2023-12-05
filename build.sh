#!/bin/bash

set -eu

mkdir -p build/
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . -j 16

