cmake_minimum_required(VERSION 3.28)
project(operations-research-two-tsp C)

set(CMAKE_C_STANDARD 99)

# Aggiungi directory di inclusione senza usare path relativi nei file sorgente
include_directories(src/constants src/parsing)

# Aggiungi i file sorgenti
set(SOURCES
        src/main.c
        src/chrono.c
        src/tsp/tsp_utilities.c
        src/tsp/tsp_utilities.h
        src/parsing/parsing_util.c
        src/parsing/parsing_util.h
)

# Definisci l'eseguibile
add_executable(tsp_solver ${SOURCES})

# Aggiungi gli header pubblici per evitare include con path relativi nei sorgenti
target_include_directories(tsp_solver PRIVATE
        ${CMAKE_SOURCE_DIR}/src/constants
        ${CMAKE_SOURCE_DIR}/src/parsing
        ${CMAKE_SOURCE_DIR}/src/tsp
)
