cmake_minimum_required(VERSION 3.16)
project(cflaglib_tests C)

# Define test source files
add_executable(cflaglib_tests test_parser.c)

# Link the test executable with cflaglib and math library
target_link_libraries(cflaglib_tests PRIVATE cflaglib m)

# Add the test to CTest
add_test(NAME cflaglib_tests COMMAND $<TARGET_FILE:cflaglib_tests>)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")