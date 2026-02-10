#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Run one scheduling pass: find a hot MemoryOS allocation with heat >= min_heat and execute a GPU in-place task on it.
 * Returns 0 on success (task executed), -1 if nothing found or on error.
 */
int scheduler_run_once(uint32_t min_heat);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULER_H */