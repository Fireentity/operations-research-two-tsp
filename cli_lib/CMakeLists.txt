# cli_lib/CMakeLists.txt
project(cli_lib)

set(CLI_LIB_SOURCES
        src/flag.c
        src/parsing_result.c
)

add_library(cli_lib ${CLI_LIB_SOURCES})
target_include_directories(cli_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
