set(_CPLEX_ROOT_HINTS)

if(CPLEX_ROOT)
    list(APPEND _CPLEX_ROOT_HINTS "${CPLEX_ROOT}")
endif()

if(DEFINED ENV{CPLEX_ROOT})
    list(APPEND _CPLEX_ROOT_HINTS "$ENV{CPLEX_ROOT}")
endif()

list(APPEND _CPLEX_ROOT_HINTS
        /opt/ibm/ILOG/
        /opt/ibm/ILOG/CPLEX_Studio2211/
        /Applications
        "C:/IBM/ILOG"
        "C:/Program Files/IBM/ILOG"
)

find_path(CPLEX_INCLUDE_DIR
        NAMES ilcplex/cplex.h
        HINTS ${_CPLEX_ROOT_HINTS}
        PATH_SUFFIXES
        .
        cplex/include
        CPLEX_Studio*/cplex/include
)

find_library(CPLEX_LIBRARY
        NAMES cplex libcplex
        HINTS ${_CPLEX_ROOT_HINTS}
        PATH_SUFFIXES
        cplex/lib/x86-64_linux/static_pic
        cplex/lib/x86-64_linux/shared_pic
        cplex/lib/x86-64_osx/static_pic
        cplex/lib/x86-64_osx/shared_pic
        cplex/lib/arm64_osx/static_pic
        cplex/lib/arm64_osx/shared_pic
        cplex/lib/x64_windows_msvc14/stat_mda
        cplex/lib
        .
)

find_package(Threads REQUIRED)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPLEX
        REQUIRED_VARS
        CPLEX_INCLUDE_DIR
        CPLEX_LIBRARY
)

if(CPLEX_FOUND AND NOT TARGET CPLEX::CPLEX)
    add_library(CPLEX::CPLEX INTERFACE IMPORTED)

    set_target_properties(CPLEX::CPLEX PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${CPLEX_LIBRARY};Threads::Threads"
    )

    if(UNIX AND NOT APPLE)
        set_property(TARGET CPLEX::CPLEX APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES "m;dl"
        )
    endif()
endif()

mark_as_advanced(CPLEX_INCLUDE_DIR CPLEX_LIBRARY)
