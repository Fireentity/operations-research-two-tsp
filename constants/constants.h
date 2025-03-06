#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "parsing_util.h"

long* set_nodes(TspParams *p) { return &p->number_of_nodes; }
long* set_seed(TspParams *p) { return &p->seed; }
long* set_x_square(TspParams *p) { return &p->rectangle.x_square; }
long* set_y_square(TspParams *p) { return &p->rectangle.y_square; }
long* set_square_side(TspParams *p) { return &p->rectangle.square_side; }
long* set_help(TspParams *p) { return &p->help; }

const CommandLineFlag tsp_flags[] = {
    {"-nodes", set_nodes},
    {"-seed", set_seed},
    {"-x_square", set_x_square},
    {"-y_square", set_y_square},
    {"-square_side", set_square_side},
    {"-help", set_help},
};

#endif //CONSTANTS_H
