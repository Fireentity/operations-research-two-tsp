# common/CMakeLists.txt
project(common)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ")

set(COMMON_SOURCES
        include/c_util.h
        src/chrono.c
        src/hashmap.c
        include/uthash.h
)

add_library(common ${COMMON_SOURCES})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cli_lib PRIVATE common)
