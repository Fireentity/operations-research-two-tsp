#DISABLE_VERBOSE
option(DISABLE_VERBOSE "Disable verbose output logging" OFF)

if (DISABLE_VERBOSE)
    add_compile_definitions(DISABLE_VERBOSE)
    message(STATUS "[Options] Verbose logging: DISABLED")
else ()
    message(STATUS "[Options] Verbose logging: ENABLED")
endif ()


#TSP_DEBUG
add_compile_definitions($<$<CONFIG:Debug>:TSP_DEBUG>)

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPER_BUILD_TYPE)
if (UPPER_BUILD_TYPE STREQUAL "DEBUG")
    message(STATUS "[Options] Memory Safety Tracking: ENABLED (Debug Build)")
else ()
    message(STATUS "[Options] Memory Safety Tracking: DISABLED (Release/RelWithDebInfo)")
endif ()