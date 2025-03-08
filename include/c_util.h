//
// Created by albyb on 08/03/2025.
//

#ifndef OPERATIONS_RESEARCH_TWO_TSP_C_UTIL_H
#define OPERATIONS_RESEARCH_TWO_TSP_C_UTIL_H
#include <stdio.h>
#include <stdlib.h>

void check_alloc(void *ptr) {
    if (!ptr) {
        perror("Errore di allocazione memoria");
        exit(EXIT_FAILURE);
    }
}

#endif //OPERATIONS_RESEARCH_TWO_TSP_C_UTIL_H
