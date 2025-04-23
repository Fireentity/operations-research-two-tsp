#!/bin/bash
rm -r ./build || echo "No folder ./build found creating one ..."
cmake -G Ninja -B build
ninja -C build