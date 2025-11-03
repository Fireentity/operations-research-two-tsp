#!/bin/bash
rm -r ./build || echo "No folder ./build found, creating one ..."

VERBOSE_FLAG=""
if [[ "$1" == "disable_verbose" || "$1" == "dv" ]]; then
    VERBOSE_FLAG="-DDISABLE_VERBOSE"
    echo "Verbose mode disabled"
else
    echo "Verbose mode enabled (default)"
fi

cmake -G Ninja -B build -DCMAKE_C_FLAGS="$VERBOSE_FLAG"
ninja -C build
cp "config.ini" ./build/tsp_solver/
