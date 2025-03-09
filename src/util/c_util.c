#include "c_util.h"

void check_alloc(const void *ptr) {
    if (!ptr) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
}