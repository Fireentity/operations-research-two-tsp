#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# Directory da escludere, separati da "|"
EXCLUDE_DIRS=("vendor" "node_modules" "cmake-build-debug" "build" ".*")

# Parse the flags and arguments
OUTPUT_FILE=""
while getopts "f:" opt; do
    case "$opt" in
        f) OUTPUT_FILE="$OPTARG" ;;
        *) echo "Usage: $0 [-f <file>] [<directory>]"; exit 1 ;;
    esac
done

# Get the directory argument after parsing the flags
DIR="${@:$OPTIND:1}"

# If no directory argument is provided, default to the current directory
DIR="${DIR:-.}"

# Resolve the full absolute path of the output file to avoid any relative path issues
if [[ -n "$OUTPUT_FILE" ]]; then
    OUTPUT_FILE="$(realpath "$OUTPUT_FILE")"
fi

# Check if output file is specified and ensure it doesn't refer to the current directory
if [[ -n "$OUTPUT_FILE" && ! -e "$OUTPUT_FILE" ]]; then
    touch "$OUTPUT_FILE"
fi

# If OUTPUT_FILE is specified, redirect output to that file
if [[ -n "$OUTPUT_FILE" ]]; then
    exec > "$OUTPUT_FILE"
fi

# 1) Print the tree, escludendo le directory specificate in EXCLUDE_DIRS
# Converting the EXCLUDE_DIRS array into a string with the proper exclusion patterns
EXCLUDE_TREE_PATTERN=$(IFS='|'; echo "${EXCLUDE_DIRS[*]}")
tree "$DIR" -I "$EXCLUDE_TREE_PATTERN|$(basename "$OUTPUT_FILE")"

# Build the exclusion pattern for the find command
EXCLUDE_FIND_PATTERN=""
for dir in "${EXCLUDE_DIRS[@]}"; do
  EXCLUDE_FIND_PATTERN+=" -name \"$dir\" -o"
done

# Remove the last OR (`-o`)
EXCLUDE_FIND_PATTERN="${EXCLUDE_FIND_PATTERN% -o}"

# Run the find command correctly using eval to interpret the pattern
eval "find \"\$DIR\" -mindepth 1 \( $EXCLUDE_FIND_PATTERN \) -prune -o \
  -type f -not -path \"$(realpath "$OUTPUT_FILE")\" -print" |
while IFS= read -r file; do
    if [[ "$(realpath "$file")" == "$OUTPUT_FILE" ]]; then
        continue
    fi

    if [ ! -s "$file" ] || grep -Iq . "$file"; then
        echo
        echo "----- Path: $file -----"
        cat "$file"
    fi
done
