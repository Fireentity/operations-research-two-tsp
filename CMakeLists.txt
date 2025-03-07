cmake_minimum_required(VERSION 3.28)
project(operations-research-two-tsp C)

set(CMAKE_C_STANDARD 99)

# Aggiungi directory di inclusione senza usare path relativi nei file sorgente
include_directories(src/constants src/parsing)

# Aggiungi i file sorgenti
set(SOURCES
        src/main.c
        src/chrono.c
        src/parsing_util.c
        src/constants.c
        include/parsing_util.h
        include/constants.h
        src/nearest_neighbor.c
        include/tsp_instance.h
        include/tsp_solution.h
        include/tsp_params.h
        src/tsp_instance.cpp
        src/tsp_solution.cpp
)

# Definisci l'eseguibile
add_executable(tsp_solver ${SOURCES})

# Aggiungi gli header pubblici per evitare include con path relativi nei sorgenti
target_include_directories(tsp_solver PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
