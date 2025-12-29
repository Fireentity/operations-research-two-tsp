#include "c_util.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Recommended for thread safety in Phase 6
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

void check_popen(FILE *gp) {
    if (!gp) {
        perror("popen error");
        exit(EXIT_FAILURE);
    }
}

void check_pclose(const int status) {
    if (status == -1) {
        perror("pclose error");
        exit(EXIT_FAILURE);
    }
}

void *memdup(const void *obj, const size_t size) {
    void *ptr = tsp_malloc(size);
    memcpy(ptr, obj, size);
    return ptr;
}

void join_path(char *out, const char *dir, const char *file, size_t maxlen) {
    const size_t len = strlen(dir);
    if (len > 0 && dir[len - 1] == '/')
        snprintf(out, maxlen, "%s%s", dir, file);
    else
        snprintf(out, maxlen, "%s/%s", dir, file);
}

void str_trim(char *s) {
    if (!s || !*s) return;
    const char *start = s;
    while (*start && isspace((unsigned char) *start))
        start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char) s[len - 1])) {
        s[len - 1] = 0;
        len--;
    }
}

/* --- DEBUG MEMORY TRACKING IMPLEMENTATION --- */

#if defined(TSP_DEBUG) || defined(DEBUG)

typedef struct AllocationRecord {
    void *ptr;
    size_t size;
    const char *file;
    int line;
    struct AllocationRecord *next;
} AllocationRecord;

static AllocationRecord *g_head = NULL;
static pthread_mutex_t g_mem_mutex = PTHREAD_MUTEX_INITIALIZER;
static size_t g_total_allocated = 0;

static void track_add(void *ptr, size_t size, const char *file, int line) {
    AllocationRecord *rec = malloc(sizeof(AllocationRecord));
    // We use raw malloc here to avoid infinite recursion!
    if (!rec) {
        fprintf(stderr, "[MEM] Critical: Failed to allocate tracker node\n");
        exit(EXIT_FAILURE);
    }
    rec->ptr = ptr;
    rec->size = size;
    rec->file = file;
    rec->line = line;

    pthread_mutex_lock(&g_mem_mutex);
    rec->next = g_head;
    g_head = rec;
    g_total_allocated += size;
    pthread_mutex_unlock(&g_mem_mutex);
}

static void track_remove(void *ptr) {
    pthread_mutex_lock(&g_mem_mutex);
    AllocationRecord *curr = g_head;
    AllocationRecord *prev = NULL;

    while (curr) {
        if (curr->ptr == ptr) {
            if (prev) prev->next = curr->next;
            else g_head = curr->next;

            g_total_allocated -= curr->size;
            free(curr); // Raw free
            pthread_mutex_unlock(&g_mem_mutex);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    pthread_mutex_unlock(&g_mem_mutex);
    // If we reach here, we freed a pointer we didn't track (or double free).
    // Optional: Print a warning.
}

void *_tsp_malloc_dbg(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "[MEM] Malloc failed at %s:%d\n", file, line);
        exit(EXIT_FAILURE);
    }
    track_add(ptr, size, file, line);
    return ptr;
}

void *_tsp_calloc_dbg(size_t num, size_t size, const char *file, int line) {
    void *ptr = calloc(num, size);
    if (!ptr && (num * size) > 0) {
        fprintf(stderr, "[MEM] Calloc failed at %s:%d\n", file, line);
        exit(EXIT_FAILURE);
    }
    track_add(ptr, num * size, file, line);
    return ptr;
}

void *_tsp_realloc_dbg(void *ptr, size_t size, const char *file, int line) {
    if (ptr) track_remove(ptr);
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        fprintf(stderr, "[MEM] Realloc failed at %s:%d\n", file, line);
        exit(EXIT_FAILURE);
    }
    track_add(new_ptr, size, file, line);
    return new_ptr;
}

void _tsp_free_dbg(void *ptr) {
    if (!ptr) return;
    track_remove(ptr);
    free(ptr);
}

void tsp_dump_memory_leaks(void) {
    pthread_mutex_lock(&g_mem_mutex);
    if (g_head == NULL) {
        fprintf(stderr, "\n[MEM] No memory leaks detected. Great job!\n");
    } else {
        fprintf(stderr, "\n[MEM] !!! MEMORY LEAKS DETECTED !!!\n");
        AllocationRecord *curr = g_head;
        while (curr) {
            fprintf(stderr, " - Leak %zu bytes at %p (Allocated: %s:%d)\n",
                    curr->size, curr->ptr, curr->file, curr->line);
            curr = curr->next;
        }
        fprintf(stderr, "[MEM] Total leaked: %zu bytes\n\n", g_total_allocated);
    }
    pthread_mutex_unlock(&g_mem_mutex);
}

long get_max_threads(void) {
#if defined(_WIN32)
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (long) info.dwNumberOfProcessors;
#elif defined(_SC_NPROCESSORS_ONLN)
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    return n > 0 ? n : 1;
#else
    return 1;
#endif
}

#endif
