#ifndef C_UTIL_H
#define C_UTIL_H
#include <stdio.h>
#include <stdlib.h>

void check_alloc(const void *ptr);
void check_popen(FILE *gp);
void check_pclose(int status);

#endif //C_UTIL_H

