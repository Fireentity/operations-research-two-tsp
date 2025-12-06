project(common)

# Set compile options for common
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for common
set(COMMON_SOURCES
        src/chrono.c
        src/ini.c
        src/logger.c
        src/c_util.c
        src/random.c
)

# Create a library for common
add_library(common ${COMMON_SOURCES})

# Include directories for common
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link common to cflaglib
target_link_libraries(common PRIVATE m)
