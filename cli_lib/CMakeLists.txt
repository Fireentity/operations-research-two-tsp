# cli_lib/CMakeLists.txt
project(cli_lib)

set(CLI_LIB_SOURCES
        src/flag.c
        src/parsing_result.c
        src/parsing_util.c
)

add_library(cli_lib INTERFACE)
target_include_directories(cli_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cli_lib INTERFACE common)
