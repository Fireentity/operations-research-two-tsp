#include "c_util.h"

void check_alloc(const void *ptr) {
    if (!ptr) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
}

void check_popen(FILE *gp) {
    if (!gp) {
        perror("popen error");
        exit(EXIT_FAILURE);
    }
}

void check_pclose(int status) {
    if (status == -1) {
        perror("pclose error");
        exit(EXIT_FAILURE);
    }
}