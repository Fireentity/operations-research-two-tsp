project(cflaglib)

# Set compile options for cflaglib
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for cflaglib
set(CFLAGLIB_SOURCES
        src/flag_parser.c
        src/parsing_util.c
        src/parsing_result.c
)

# Create a library for cflaglib
add_library(cflaglib ${CFLAGLIB_SOURCES})

# Include directories for cflaglib
target_include_directories(cflaglib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link with common library
target_link_libraries(cflaglib PUBLIC common m)
