#ifndef ALGORITHM_CONSTANTS_H
#define ALGORITHM_CONSTANTS_H

#define EPSILON 1e-10

#define WITH_MUTEX(mutex, code) do { \
pthread_mutex_lock(mutex);     \
code;                          \
pthread_mutex_unlock(mutex);   \
} while(0)
#endif //ALGORITHM_CONSTANTS_H
