#!/bin/bash
rm -r ./build || echo "No folder ./build found, creating one ..."

VERBOSE_OPTION="-DDISABLE_VERBOSE=OFF"
if [[ "$1" == "disable_verbose" || "$1" == "dv" ]]; then
    VERBOSE_OPTION="-DDISABLE_VERBOSE=ON"
fi

cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      $VERBOSE_OPTION

ninja -C build
cp "config.ini" ./build/tsp_solver/