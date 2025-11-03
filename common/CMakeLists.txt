project(common)

# Set compile options for common
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for common
set(COMMON_SOURCES
        include/c_util.h
        src/chrono.c
        src/hashmap.c
        include/uthash.h
        tests/str_format_test.c
        include/ini.h
        src/ini.c
)

# Create a library for common
add_library(common ${COMMON_SOURCES})

# Include directories for common
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link common to cflaglib
target_link_libraries(common PRIVATE m)
