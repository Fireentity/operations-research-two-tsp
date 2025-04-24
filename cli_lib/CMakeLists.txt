project(cli_lib)

# Set compile options for cli_lib
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for cli_lib
set(CLI_LIB_SOURCES
        src/flag.c
        src/flag_parser.c
        src/parsing_util.c
        src/parsing_result.c
)

# Create a library for cli_lib
add_library(cli_lib ${CLI_LIB_SOURCES})

# Include directories for cli_lib
target_include_directories(cli_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link with common library
target_link_libraries(cli_lib PUBLIC common m)
