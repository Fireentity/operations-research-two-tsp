cmake_minimum_required(VERSION 3.16.1)
project(operations-research-two-tsp C)

set(CMAKE_C_STANDARD 99)

# Aggiungi directory di inclusione senza usare path relativi nei file sorgente
include_directories(src/flag include/flag src/flag include/util include/algorithms src/algorithms)

# Aggiungi i file sorgenti
set(SOURCES
        src/util/parsing_util.c
        src/util/math_util.c
        src/util/c_util.c
        src/tsp_instance.c
        src/tsp_solution.c
        src/cmd_options.c
        src/main.c
        src/util/chrono.c
        src/util/plot_util.c
        src/enums.c
        src/flag/flag.c
        src/algorithms/nearest_neighbor.c
        src/algorithms/tsp_algorithm.c

        include/util/parsing_util.h
        include/util/math_util.h
        include/util/c_util.h
        include/flag/flag.h
        include/tsp_instance.h
        include/tsp_solution.h
        include/cmd_options.h
        include/enums.h
        include/constants.h
        include/util/plot_util.h
        include/util/chrono.h
        include/algorithms/nearest_neighbor.h
        include/algorithms/tsp_algorithm.h
)

# Definisci l'eseguibile
add_executable(tsp_solver ${SOURCES})

# Aggiungi gli header pubblici per evitare include con path relativi nei sorgenti
target_include_directories(tsp_solver PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tsp_solver PRIVATE m)
