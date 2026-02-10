#include "gpu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

gpu_mem_t gpu_allocate_memory(size_t size) {
    gpu_mem_t m = { NULL, 0 };
    if (size == 0) return m;
    void* p = malloc(size);
    if (!p) {
        fprintf(stderr, "gpu_allocate_memory: OOM for %zu bytes\n", size);
        return m;
    }
    m.ptr = p;
    m.size = size;
    return m;
}

void gpu_free_memory(gpu_mem_t mem) {
    if (mem.ptr) free(mem.ptr);
}

int gpu_execute_task(gpu_mem_t mem) {
    if (!mem.ptr || mem.size == 0) return -1;
    memset(mem.ptr, 0xAA, mem.size);
    return 0;
}

int gpu_execute_task_inplace(void* ptr, size_t size) {
    if (!ptr || size == 0) return -1;
    memset(ptr, 0xAA, size);
    return 0;
}
