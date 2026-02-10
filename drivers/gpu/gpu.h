#ifndef GPU_H
#define GPU_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpu_mem {
    void* ptr;
    size_t size;
} gpu_mem_t;

/** Allocate a chunk of GPU memory (mocked). Returns a gpu_mem_t with ptr==NULL on failure. */
gpu_mem_t gpu_allocate_memory(size_t size);

/** Free GPU memory allocated by gpu_allocate_memory. Safe to call with ptr==NULL. */
void gpu_free_memory(gpu_mem_t mem);

/** Execute a GPU task against the memory region (mocked). Returns 0 on success. */
int gpu_execute_task(gpu_mem_t mem);

/** Execute a GPU task *in-place* against a user-provided memory pointer. This is used to operate on MemoryOS pool memory without copying. Returns 0 on success. */
int gpu_execute_task_inplace(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* GPU_H */
