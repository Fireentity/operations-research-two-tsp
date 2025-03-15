#!/bin/bash

git clean -fdx -e .idea
mkdir build && cd ./build
cmake ../
cmake --build .
ctest --output-on-failure
