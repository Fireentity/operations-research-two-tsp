cmake_minimum_required(VERSION 3.16.1)
project(operations-research-two-tsp C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

option(DISABLE_VERBOSE "Disable verbose mode" OFF)
if(DISABLE_VERBOSE)
    message(STATUS "Verbose mode disabled")
    add_compile_definitions(DISABLE_VERBOSE)
else()
    message(STATUS "Verbose mode enabled (default)")
endif()

# Enable testing
enable_testing()

# Add subdirectories for each module
add_subdirectory(cflaglib)
add_subdirectory(cflaglib/tests)
add_subdirectory(tsp_algo_lib)
add_subdirectory(tsp_algo_lib/tests)
add_subdirectory(tsp_solver)
add_subdirectory(common)
