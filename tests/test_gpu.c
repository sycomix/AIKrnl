#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../drivers/gpu/gpu.h"

int main(void) {
    size_t n = 64;
    gpu_mem_t mem = gpu_allocate_memory(n);
    if (!mem.ptr) { printf("gpu_alloc failed\n"); return 1; }
    if (gpu_execute_task(mem) != 0) { printf("gpu_execute_task failed\n"); gpu_free_memory(mem); return 1; }
    unsigned char* p = (unsigned char*)mem.ptr;
    for (size_t i = 0; i < n; ++i) {
        if (p[i] != 0xAA) { printf("pattern mismatch at %zu: %02x\n", i, p[i]); gpu_free_memory(mem); return 1; }
    }
    gpu_free_memory(mem);
    printf("GPU mock test passed\n");
    return 0;
}
