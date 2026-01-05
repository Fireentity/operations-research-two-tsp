#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"

# Default Values
BUILD_TYPE="Release"
CLEAN_BUILD=false
RUN_TESTS=false
VERBOSE_DEF="-DDISABLE_VERBOSE=OFF" # Verbose is ON by default
CPLEX_DEF=""

#  Argument Parsing 
for arg in "$@"; do
    case "$arg" in
        --debug)
            BUILD_TYPE="Debug"
            ;;
        --clean)
            CLEAN_BUILD=true
            ;;
        --test)
            RUN_TESTS=true
            ;;
        --quiet|--no-verbose)
            VERBOSE_DEF="-DDISABLE_VERBOSE=ON"
            ;;
        --cplex=*|cplex=*)
            # Supports both --cplex=/path and cplex=/path
            CPLEX_PATH="${arg#*=}"
            CPLEX_DEF="-DCPLEX_ROOT=$CPLEX_PATH"
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --debug          Build in Debug mode (default: Release)"
            echo "  --clean          Remove build directory before compiling"
            echo "  --test           Run tests after building"
            echo "  --quiet          Disable verbose logging in the application"
            echo "  --cplex=<path>   Specify CPLEX root directory"
            exit 0
            ;;
        *)
            echo "Error: Unknown argument '$arg'"
            exit 1
            ;;
    esac
done

#  Clean Step 
if [ "$CLEAN_BUILD" = true ]; then
    echo "[Build] Cleaning build directory..."
    rm -rf build
fi

#  Configure Step 
echo "[Build] Configuring project ($BUILD_TYPE mode)..."
if [ -n "$CPLEX_DEF" ]; then
    echo "[Build] Using CPLEX path: ${CPLEX_DEF#*=}"
fi

cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      $VERBOSE_DEF \
      $CPLEX_DEF

#  Compile Step 
echo "[Build] Compiling..."
ninja -C build

#  Post-Build Operations 
# Copy the configuration file only if it exists
if [ -f "config.ini" ]; then
    echo "[Build] Copying config.ini..."
    cp config.ini ./build/tsp_solver/
fi

#  Test Step 
if [ "$RUN_TESTS" = true ]; then
    echo "[Build] Running Tests..."
    cd build
    ctest --output-on-failure
fi

echo "[Build] Done."