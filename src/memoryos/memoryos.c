#define _POSIX_C_SOURCE 200809L
#include <memoryos.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define MAGIC 0xA1050501u
#define GRANULARITY 64
#define MAX_ALLOCS 1024
#define MAX_DOCS 256

typedef struct alloc_header {
    uint32_t magic;
    uint32_t id;
    uint64_t size;
} alloc_header_t;

typedef struct alloc_meta {
    size_t start_block;
    size_t num_blocks;
    uint32_t heat;
    int tier; // 0 LPM,1 MTM,2 STM
    int in_use;
    int reserved; /* reserved by scheduler to avoid free races */
} alloc_meta_t;

static unsigned char* pool = NULL;
static size_t pool_size = 0;
static size_t num_blocks = 0;
static uint8_t* bitmap = NULL; // 1 = used
static alloc_meta_t metas[MAX_ALLOCS];
static int next_alloc_id = 1;

/* Mutex to protect pool/bitmap/metas/docs for concurrency */
static pthread_mutex_t mem_lock;
static int mem_lock_inited = 0;

/* Semantic doc */
typedef struct doc_t {
    char* id;
    float emb[P2OS_EMBED_DIM];
    void* data;
    size_t size;
    int in_use;
} doc_t;

static doc_t docs[MAX_DOCS];



int memoryos_init(size_t pool_size_bytes) {
    if (pool) return -1; // already init
    if (pool_size_bytes < 4096) return -1;
    pool_size = (pool_size_bytes + GRANULARITY - 1) & ~(GRANULARITY - 1);
    pool = malloc(pool_size);
    if (!pool) return -1;
    num_blocks = pool_size / GRANULARITY;
    bitmap = calloc(num_blocks, 1);
    if (!bitmap) { free(pool); pool = NULL; return -1; }
    memset(metas, 0, sizeof(metas));
    memset(docs, 0, sizeof(docs));
    next_alloc_id = 1;
    if (!mem_lock_inited) { pthread_mutex_init(&mem_lock, NULL); mem_lock_inited = 1; }
    return 0;
}

void memoryos_shutdown(void) {
    if (!pool) return;
    if (mem_lock_inited) { pthread_mutex_lock(&mem_lock); }
    for (int i = 0; i < MAX_DOCS; ++i) {
        if (docs[i].in_use) {
            free(docs[i].id);
            docs[i].in_use = 0;
        }
    }
    free(bitmap); bitmap = NULL;
    free(pool); pool = NULL;
    pool_size = 0; num_blocks = 0;
    if (mem_lock_inited) { pthread_mutex_unlock(&mem_lock); pthread_mutex_destroy(&mem_lock); mem_lock_inited = 0; }
}

static int find_free_run(size_t blocks_needed) {
    size_t run = 0;
    for (size_t i = 0; i < num_blocks; ++i) {
        if (!bitmap[i]) run++; else run = 0;
        if (run == blocks_needed) return (int)(i + 1 - run);
    }
    return -1;
}

void* memoryos_alloc(size_t size) {
    if (!pool || size == 0) return NULL;
    size_t total = size + sizeof(alloc_header_t);
    size_t blocks = (total + GRANULARITY - 1) / GRANULARITY;
    int start = find_free_run(blocks);
    if (start < 0) return NULL;
    for (size_t i = 0; i < blocks; ++i) bitmap[start + i] = 1;
    // find free meta slot
    int meta_id = -1;
    for (int i = 0; i < MAX_ALLOCS; ++i) if (!metas[i].in_use) { meta_id = i; break; }
    if (meta_id < 0) { // rollback
        for (size_t i = 0; i < blocks; ++i) bitmap[start + i] = 0;
        return NULL;
    }
    metas[meta_id].in_use = 1;
    metas[meta_id].start_block = start;
    metas[meta_id].num_blocks = blocks;
    metas[meta_id].heat = 0;
    metas[meta_id].tier = 0; // start in LPM

    unsigned char* base = pool + start * GRANULARITY;
    alloc_header_t* h = (alloc_header_t*)base;
    h->magic = MAGIC;
    h->id = (uint32_t)(meta_id + 1);
    h->size = size;
    void* user_ptr = base + sizeof(alloc_header_t);
    return user_ptr;
}

static alloc_meta_t* meta_for_ptr(void* ptr) {
    if (!ptr || !pool) return NULL;
    unsigned char* p = (unsigned char*)ptr;
    if (p < pool || p >= pool + pool_size) return NULL;
    unsigned char* base = p - sizeof(alloc_header_t);
    alloc_header_t* h = (alloc_header_t*)base;
    if (h->magic != MAGIC) return NULL;
    uint32_t id = h->id;
    if (id == 0 || id > (uint32_t)MAX_ALLOCS) return NULL;
    alloc_meta_t* m = &metas[id - 1];
    if (!m->in_use) return NULL;
    return m;
}

void memoryos_free(void* ptr) {
    if (!ptr) return;
    pthread_mutex_lock(&mem_lock);
    alloc_meta_t* m = meta_for_ptr(ptr);
    if (!m) { pthread_mutex_unlock(&mem_lock); return; }
    if (m->reserved) { /* scheduler reserved it; cannot free right now */ pthread_mutex_unlock(&mem_lock); return; }
    size_t start = m->start_block;
    for (size_t i = 0; i < m->num_blocks; ++i) bitmap[start + i] = 0;
    m->in_use = 0;
    pthread_mutex_unlock(&mem_lock);
}

void memoryos_touch(void* ptr) {
    if (!ptr) return;
    pthread_mutex_lock(&mem_lock);
    alloc_meta_t* m = meta_for_ptr(ptr);
    if (!m) { pthread_mutex_unlock(&mem_lock); return; }
    m->heat += 1;
    // Promotion heuristic: simple thresholds for prototype
    if (m->heat > 50 && m->tier < 2) m->tier++;
    if (m->heat > 200 && m->tier < 2) m->tier = 2; // hot items become STM
    pthread_mutex_unlock(&mem_lock);
}

int memoryos_get_tier(void* ptr) {
    if (!ptr) return -1;
    pthread_mutex_lock(&mem_lock);
    alloc_meta_t* m = meta_for_ptr(ptr);
    if (!m) { pthread_mutex_unlock(&mem_lock); return -1; }
    int t = m->tier;
    pthread_mutex_unlock(&mem_lock);
    return t;
}

/* Simple semantic index: store doc in pool and its embedding */
int p2os_index_put(const char* id, const float* embedding, const void* data, size_t data_size) {
    if (!id || !embedding || !data) return -1;
    pthread_mutex_lock(&mem_lock);
    // find free doc slot
    int slot = -1;
    for (int i = 0; i < MAX_DOCS; ++i) if (!docs[i].in_use) { slot = i; break; }
    if (slot < 0) { pthread_mutex_unlock(&mem_lock); return -1; }
    // store data in pool
    pthread_mutex_unlock(&mem_lock);
    void* mem = memoryos_alloc(data_size);
    if (!mem) return -1;
    pthread_mutex_lock(&mem_lock);
    memcpy(mem, data, data_size);
    docs[slot].id = strdup(id);
    for (int d = 0; d < P2OS_EMBED_DIM; ++d) docs[slot].emb[d] = embedding[d];
    docs[slot].data = mem;
    docs[slot].size = data_size;
    docs[slot].in_use = 1;
    pthread_mutex_unlock(&mem_lock);
    return 0;
}

int p2os_get_data(const char* id, void** out_data, size_t* out_size) {
    if (!id || !out_data || !out_size) return -1;
    pthread_mutex_lock(&mem_lock);
    for (int i = 0; i < MAX_DOCS; ++i) {
        if (!docs[i].in_use) continue;
        if (strcmp(docs[i].id, id) == 0) {
            *out_data = docs[i].data;
            *out_size = docs[i].size;
            pthread_mutex_unlock(&mem_lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&mem_lock);
    return -1;
}

static float dot(const float* a, const float* b, size_t n) {
    float s = 0.0f;
    for (size_t i = 0; i < n; ++i) s += a[i] * b[i];
    return s;
}

size_t p2os_query(const float* query_embedding, size_t k, char** out_ids, float* out_scores) {
    if (!query_embedding || k == 0 || !out_ids || !out_scores) return 0;
    // simple linear scan
    size_t found = 0;
    pthread_mutex_lock(&mem_lock);
    for (int i = 0; i < MAX_DOCS; ++i) {
        if (!docs[i].in_use) continue;
        float sc = dot(query_embedding, docs[i].emb, P2OS_EMBED_DIM);
        // insert into top-k (simple insertion sort)
        size_t j = found;
        if (found < k) {
            out_ids[found] = docs[i].id;
            out_scores[found] = sc;
            found++;
            j = found - 1;
        } else if (sc > out_scores[found - 1]) {
            // replace last then bubble
            out_ids[found - 1] = docs[i].id;
            out_scores[found - 1] = sc;
            j = found - 1;
        } else continue;
        // bubble down to maintain descending order
        while (j > 0 && out_scores[j] > out_scores[j - 1]) {
            // swap
            char* tid = out_ids[j - 1]; float tsc = out_scores[j - 1];
            out_ids[j - 1] = out_ids[j]; out_scores[j - 1] = out_scores[j];
            out_ids[j] = tid; out_scores[j] = tsc;
            j--;
        }
    }
    pthread_mutex_unlock(&mem_lock);
    return found;
}

int memoryos_pick_hot(uint32_t min_heat, void** out_ptr, size_t* out_size) {
    if (!pool || !out_ptr || !out_size) return -1;
    pthread_mutex_lock(&mem_lock);
    int best = -1;
    uint32_t best_heat = 0;
    for (int i = 0; i < MAX_ALLOCS; ++i) {
        if (!metas[i].in_use) continue;
        if (metas[i].heat >= min_heat && metas[i].heat > best_heat) {
            best = i; best_heat = metas[i].heat;
        }
    }
    if (best < 0) { pthread_mutex_unlock(&mem_lock); return -1; }
    metas[best].reserved = 1; /* reserve to prevent free */
    unsigned char* base = pool + metas[best].start_block * GRANULARITY;
    alloc_header_t* h = (alloc_header_t*)base;
    *out_ptr = base + sizeof(alloc_header_t);
    *out_size = (size_t)h->size;
    pthread_mutex_unlock(&mem_lock);
    return 0;
}

int memoryos_reset_heat(void* ptr) {
    if (!ptr) return -1;
    pthread_mutex_lock(&mem_lock);
    alloc_meta_t* m = meta_for_ptr(ptr);
    if (!m) { pthread_mutex_unlock(&mem_lock); return -1; }
    m->heat = 0;
    m->reserved = 0;
    pthread_mutex_unlock(&mem_lock);
    return 0;
}
