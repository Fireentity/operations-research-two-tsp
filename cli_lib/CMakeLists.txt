# cli_lib/CMakeLists.txt
project(cli_lib)

set(CLI_LIB_SOURCES
        src/parsing_util.c
        src/flag.c
)

add_library(cli_lib ${CLI_LIB_SOURCES})
target_include_directories(cli_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cli_lib PRIVATE common)
