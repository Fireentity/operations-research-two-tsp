#!/bin/bash
rm -r ./build
cmake -G Ninja -B build
ninja -C build