cmake_minimum_required(VERSION 3.16)
project(str_format_tests C)

# Build your test executable by compiling both the implementation
# (str_format.c) and the test harness (tests.c)
add_executable(str_format_test str_format_test.c)

# Link the math library (for things like vsnprintf, printf, etc. –
# on some platforms it lives in libm)
target_link_libraries(str_format_test PRIVATE common m)


# Enable CTest and register your single test
add_test(NAME str_format_test COMMAND $<TARGET_FILE:str_format_test>)
