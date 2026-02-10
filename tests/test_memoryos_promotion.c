#include <stdio.h>
#include "../include/memoryos.h"

int main(void) {
    if (memoryos_init(64 * 1024) != 0) { printf("memoryos_init failed\n"); return 1; }
    void* p = memoryos_alloc(128);
    if (!p) { printf("alloc failed\n"); return 1; }
    int tier0 = memoryos_get_tier(p);
    for (int i = 0; i < 200; ++i) {
        memoryos_touch(p);
    }
    int tier1 = memoryos_get_tier(p);
    if (tier1 <= tier0) { printf("promotion failed: %d -> %d\n", tier0, tier1); return 1; }
    memoryos_free(p);
    memoryos_shutdown();
    printf("Memory promotion test passed\n");
    return 0;
}
