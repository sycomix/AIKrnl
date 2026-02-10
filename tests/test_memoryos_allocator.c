#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/memoryos.h"

int main(void) {
    if (memoryos_init(64 * 1024) != 0) { printf("memoryos_init failed\n"); return 1; }

    void* a = memoryos_alloc(128);
    void* b = memoryos_alloc(256);
    void* c = memoryos_alloc(3000);
    if (!a || !b || !c) { printf("alloc failed\n"); return 1; }

    uintptr_t a0 = (uintptr_t)a, a1 = a0 + 128;
    uintptr_t b0 = (uintptr_t)b, b1 = b0 + 256;
    uintptr_t c0 = (uintptr_t)c, c1 = c0 + 3000;

    if ((a0 < b1 && b0 < a1) || (a0 < c1 && c0 < a1) || (b0 < c1 && c0 < b1)) {
        printf("overlap detected\n"); return 1;
    }

    memoryos_free(b);
    void* d = memoryos_alloc(200);
    if (!d) { printf("alloc after free failed\n"); return 1; }

    memoryos_free(a); memoryos_free(c); memoryos_free(d);
    memoryos_shutdown();
    printf("Memory allocator tests passed\n");
    return 0;
}
