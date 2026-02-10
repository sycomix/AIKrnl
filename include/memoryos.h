#ifndef MEMORYOS_H
#define MEMORYOS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define P2OS_EMBED_DIM 8

int memoryos_init(size_t pool_size_bytes);
void memoryos_shutdown(void);

/** Allocate memory from the zero-copy pool. Returns NULL on failure. */
void* memoryos_alloc(size_t size);

/** Free memory previously allocated from the pool. Safe to free NULL. */
void memoryos_free(void* ptr);

/** Touch an allocation to increase its "heat" (used for promotion). */
void memoryos_touch(void* ptr);

/** Get tier for an allocation: 0=LPM,1=MTM,2=STM, -1 on error. */
int memoryos_get_tier(void* ptr);

/* Simple semantic index (P2OS) API */
int p2os_index_put(const char* id, const float* embedding /* length P2OS_EMBED_DIM */, const void* data, size_t data_size);

/**
 * Query the semantic index. Returns number of results (<= k). Caller must provide out_ids as char*[] with capacity k and out_scores as float[k].
 */
size_t p2os_query(const float* query_embedding, size_t k, char** out_ids, float* out_scores);

/** Get pointer/size for a previously indexed doc by id. Returns 0 on success, -1 otherwise. */
int p2os_get_data(const char* id, void** out_data, size_t* out_size);
/** Pick a hot allocation with heat >= min_heat. Returns 0 and fills out_ptr/out_size on success, -1 if none found. */
int memoryos_pick_hot(uint32_t min_heat, void** out_ptr, size_t* out_size);

/** Reset the heat counter for an allocation (used after scheduling). Returns 0 on success, -1 on failure. */
int memoryos_reset_heat(void* ptr);
#ifdef __cplusplus
}
#endif

#endif /* MEMORYOS_H */