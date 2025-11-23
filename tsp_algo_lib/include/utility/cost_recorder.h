#ifndef COST_RECORDER_H
#define COST_RECORDER_H

#include <stddef.h>

typedef struct CostRecorder CostRecorder;

CostRecorder *cost_recorder_create(size_t initial_capacity);

void cost_recorder_destroy(CostRecorder *r);

void cost_recorder_add(CostRecorder *r, double cost);


// TODO complete with getter for costs e for enable
#endif
