cmake_minimum_required(VERSION 3.16)
project(cli_lib_tests C)

# Define test source files
add_executable(cli_lib_tests flag_parser_test.c)

# Link the test executable with cli_lib and math library
target_link_libraries(cli_lib_tests PRIVATE cli_lib m)

# Add the test to CTest
add_test(NAME cli_lib_tests COMMAND $<TARGET_FILE:cli_lib_tests>)
