cmake_minimum_required(VERSION 3.25.1)
project(operations-research-two-tsp C)

set(CMAKE_C_STANDARD 99)

# Aggiungi directory di inclusione senza usare path relativi nei file sorgente
include_directories(src/constants src/parsing)

# Aggiungi i file sorgenti
set(SOURCES
        src/main.c
        src/chrono.c
        include/constants.h
        include/tsp_instance.h
        include/tsp_solution.h
        include/tsp_params.h
        src/tsp_params.c
        src/tsp_instance.c
        src/tsp_solution.c
        include/math_util.h
        src/math_util.c
        include/enums.h
        src/parsing_util.c
        include/parsing_util.h
        include/c_util.h
        src/c_util.c
        src/enums.c
)

# Definisci l'eseguibile
add_executable(tsp_solver ${SOURCES})

# Aggiungi gli header pubblici per evitare include con path relativi nei sorgenti
target_include_directories(tsp_solver PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tsp_solver PRIVATE m)
