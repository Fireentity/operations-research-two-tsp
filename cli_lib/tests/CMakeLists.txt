cmake_minimum_required(VERSION 3.16)
project(cli_lib_tests C)

add_executable(cli_lib_tests flag_parser_test.c
)

target_link_libraries(cli_lib_tests PRIVATE cli_lib m )

add_test(NAME cli_lib_tests COMMAND $<TARGET_FILE:cli_lib_tests>)
