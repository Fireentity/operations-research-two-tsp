cmake_minimum_required(VERSION 3.16.1)
project(operations-research-two-tsp LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_definitions(_POSIX_C_SOURCE=200809L)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ProjectOptions)

add_compile_options(
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-Wall>
        $<$<CONFIG:Debug>:-Wextra>
        $<$<CONFIG:Release>:-O3>
)

find_package(CPLEX QUIET)

if(CPLEX_FOUND)
    add_compile_definitions(ENABLE_CPLEX)
    message(STATUS "[CPLEX] Found. Exact algorithms enabled.")
else()
    message(WARNING "[CPLEX] Not found. Exact algorithms disabled.")
endif()

enable_testing()

add_subdirectory(common)
add_subdirectory(cflaglib)
add_subdirectory(cflaglib/tests)
add_subdirectory(tsp_algo_lib)
add_subdirectory(tsp_algo_lib/tests)
add_subdirectory(tsp_solver)
