#include "cost_recorder.h"
#include <stdlib.h>
#include "c_util.h"

struct CostRecorder {
    double *data;
    size_t count;
    size_t capacity;
    bool enabled;
};

CostRecorder *cost_recorder_create(size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 1024;

    CostRecorder *r = tsp_malloc(sizeof(CostRecorder));


    r->data = tsp_malloc(initial_capacity * sizeof(double));


    r->count = 0;
    r->capacity = initial_capacity;
    r->enabled = true;

    return r;
}

void cost_recorder_destroy(CostRecorder *r) {
    if (!r) return;

    tsp_free(r->data);
    tsp_free(r);
}

void cost_recorder_add(CostRecorder *r, const double cost) {
    if (!r || !r->enabled) return;

    if (r->count == r->capacity) {
        const size_t new_cap = r->capacity * 2;
        double *tmp = tsp_realloc(r->data, new_cap * sizeof(double));

        r->data = tmp;
        r->capacity = new_cap;
    }

    r->data[r->count++] = cost;
}

size_t cost_recorder_get_count(const CostRecorder *r) {
    if (!r) return 0;
    return r->count;
}

const double *cost_recorder_get_costs(const CostRecorder *r) {
    if (!r) return NULL;
    return r->data;
}

void cost_recorder_enable(CostRecorder *r) {
    if (!r) return;
    r->enabled = true;
}

void cost_recorder_disable(CostRecorder *r) {
    if (!r) return;
    r->enabled = false;
}
