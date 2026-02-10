#include <stdio.h>
#include <string.h>
#include "../include/memoryos.h"
#include "../src/scheduler/scheduler.h"

int main(void) {
    if (memoryos_init(64 * 1024) != 0) { printf("memoryos_init failed\n"); return 1; }

    void* a = memoryos_alloc(256);
    void* b = memoryos_alloc(256);
    if (!a || !b) { printf("alloc failed\n"); return 1; }
    // ensure b is zeroed (pool may not be initialized to zero in prototype)
    memset(b, 0, 256);

    // make a hot
    for (int i = 0; i < 300; ++i) memoryos_touch(a);
    int tier = memoryos_get_tier(a);
    if (tier < 1) { printf("a did not promote enough, tier=%d\n", tier); return 1; }

    // run scheduler once with threshold that picks hot entries (e.g., >=100)
    if (scheduler_run_once(100) != 0) { printf("scheduler_run_once failed\n"); return 1; }

    unsigned char* ap = (unsigned char*)a;
    unsigned char* bp = (unsigned char*)b;
    int a_written = 0; int b_written = 0;
    for (int i = 0; i < 256; ++i) { if (ap[i] == 0xAA) { a_written = 1; break; } }
    for (int i = 0; i < 256; ++i) { if (bp[i] == 0xAA) { b_written = 1; break; } }

    if (!a_written) { printf("scheduler did not write to hot allocation\n"); return 1; }
    if (b_written) { printf("scheduler incorrectly wrote to cold allocation\n"); return 1; }

    printf("Scheduler integration test passed\n");
    memoryos_shutdown();
    return 0;
}
