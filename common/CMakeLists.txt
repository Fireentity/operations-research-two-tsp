# common/CMakeLists.txt
project(common)

set(COMMON_SOURCES
        src/c_util.c
        src/chrono.c
        src/hashmap.c
)

add_library(common ${COMMON_SOURCES})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cli_lib PRIVATE common)
