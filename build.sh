#!/bin/bash
set -e

rm -rf ./build || true

VERBOSE_OPTION="-DDISABLE_VERBOSE=OFF"
CPLEX_OPTION=""

for arg in "$@"; do
    case "$arg" in
        disable_verbose|dv)
            VERBOSE_OPTION="-DDISABLE_VERBOSE=ON"
            ;;
        cplex=*|CPLEX=*)
            CPLEX_OPTION="-DCPLEX_ROOT=${arg#*=}"
            ;;
        /*)
            CPLEX_OPTION="-DCPLEX_ROOT=$arg"
            ;;
    esac
done

cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      $VERBOSE_OPTION \
      $CPLEX_OPTION

ninja -C build
cp config.ini ./build/tsp_solver/
