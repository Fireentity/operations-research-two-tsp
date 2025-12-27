option(DISABLE_VERBOSE "Disable verbose output logging" OFF)

if(DISABLE_VERBOSE)
    add_compile_definitions(DISABLE_VERBOSE)
    message(STATUS "[Options] Verbose logging: DISABLED")
else()
    message(STATUS "[Options] Verbose logging: ENABLED")
endif()