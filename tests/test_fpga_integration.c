#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/memoryos.h"
#include "../drivers/accel/fpga.h"

int main(void) {
    if (memoryos_init(64 * 1024) != 0) { printf("memoryos_init failed\n"); return 1; }
    size_t n = 64;
    size_t bytes = n * sizeof(double);
    double* a = memoryos_alloc(bytes);
    double* b = memoryos_alloc(bytes);
    double* out = memoryos_alloc(bytes);
    if (!a || !b || !out) { printf("alloc failed\n"); return 1; }
    for (size_t i = 0; i < n; ++i) { a[i] = (double)i; b[i] = (double)(3*i); }
    if (fpga_add_double(a,b,out,n) != 0) { printf("fpga add failed\n"); return 1; }
    for (size_t i = 0; i < n; ++i) if (out[i] != a[i] + b[i]) { printf("integration mismatch at %zu\n", i); return 1; }
    memoryos_free(a); memoryos_free(b); memoryos_free(out); memoryos_shutdown();
    printf("FPGA integration test passed\n");
    return 0;
}
