# cli_lib/CMakeLists.txt
project(cli_lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ")

set(CLI_LIB_SOURCES
        src/flag.c
        src/flag_parser.c
        src/parsing_util.c
)

add_library(cli_lib ${CLI_LIB_SOURCES})
target_include_directories(cli_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cli_lib PRIVATE common)
