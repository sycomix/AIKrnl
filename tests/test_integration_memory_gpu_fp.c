#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/memoryos.h"
#include "../include/fp_math.h"
#include "../drivers/gpu/gpu.h"

int main(void) {
    if (memoryos_init(64 * 1024) != 0) { printf("memoryos_init failed\n"); return 1; }

    // FP math on MemoryOS-backed buffers
    size_t n = 128;
    size_t bytes = n * sizeof(double);
    double* a = memoryos_alloc(bytes);
    double* b = memoryos_alloc(bytes);
    double* out = memoryos_alloc(bytes);
    if (!a || !b || !out) { printf("alloc failed\n"); return 1; }

    for (size_t i = 0; i < n; ++i) { a[i] = (double)i; b[i] = (double)(2*i); out[i] = 0.0; }
    fp_add_double(a, b, out, n);
    for (size_t i = 0; i < n; ++i) {
        if (out[i] != a[i] + b[i]) { printf("fp add mismatch at %zu\n", i); return 1; }
    }

    fp_mul_double(a, b, out, n);
    for (size_t i = 0; i < n; ++i) {
        if (out[i] != a[i] * b[i]) { printf("fp mul mismatch at %zu\n", i); return 1; }
    }

    // P2OS data -> GPU in-place
    char doc_data[64]; memset(doc_data, 0, sizeof(doc_data));
    float emb[P2OS_EMBED_DIM] = {1,0,0,0,0,0,0,0};
    if (p2os_index_put("gpu_doc", emb, doc_data, sizeof(doc_data)) != 0) { printf("index failed\n"); return 1; }

    void* doc_ptr = NULL; size_t doc_size = 0;
    if (p2os_get_data("gpu_doc", &doc_ptr, &doc_size) != 0) { printf("get_data failed\n"); return 1; }
    if (!doc_ptr || doc_size != sizeof(doc_data)) { printf("unexpected doc ptr/size\n"); return 1; }

    if (gpu_execute_task_inplace(doc_ptr, doc_size) != 0) { printf("gpu inplace failed\n"); return 1; }
    unsigned char* cp = (unsigned char*)doc_ptr;
    for (size_t i = 0; i < doc_size; ++i) if (cp[i] != 0xAA) { printf("gpu wrote wrong byte at %zu: %02x\n", i, cp[i]); return 1; }

    // Also exercise GPU inplace on a raw MemoryOS allocation
    void* raw = memoryos_alloc(128);
    if (!raw) { printf("raw alloc failed\n"); return 1; }
    if (gpu_execute_task_inplace(raw, 128) != 0) { printf("gpu inplace raw failed\n"); return 1; }
    unsigned char* rp = (unsigned char*)raw;
    for (size_t i = 0; i < 128; ++i) if (rp[i] != 0xAA) { printf("gpu wrote wrong byte to raw at %zu: %02x\n", i, rp[i]); return 1; }

    memoryos_free(a); memoryos_free(b); memoryos_free(out); memoryos_free(raw);
    memoryos_shutdown();
    printf("Integration test passed\n");
    return 0;
}
