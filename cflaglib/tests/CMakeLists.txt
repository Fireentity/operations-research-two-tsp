cmake_minimum_required(VERSION 3.16)
project(cflaglib_tests C)

add_executable(cflaglib_tests test_parser.c)
target_link_libraries(cflaglib_tests PRIVATE cflaglib m)

target_compile_options(cflaglib_tests PRIVATE -UNDEBUG)

add_test(NAME cflaglib_tests COMMAND $<TARGET_FILE:cflaglib_tests>)
