#include "scheduler.h"
#include "../include/memoryos.h"
#include "../drivers/gpu/gpu.h"

int scheduler_run_once(uint32_t min_heat) {
    void* ptr = NULL; size_t size = 0;
    if (memoryos_pick_hot(min_heat, &ptr, &size) != 0) return -1;
    if (!ptr || size == 0) return -1;
    if (gpu_execute_task_inplace(ptr, size) != 0) return -1;
    // reset heat so we don't repeatedly pick the same item
    if (memoryos_reset_heat(ptr) != 0) return -1;
    return 0;
}
