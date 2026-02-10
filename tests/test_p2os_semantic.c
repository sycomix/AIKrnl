#include <stdio.h>
#include <string.h>
#include "../include/memoryos.h"

int main(void) {
    if (memoryos_init(64 * 1024) != 0) { printf("memoryos_init failed\n"); return 1; }
    float emb1[P2OS_EMBED_DIM] = {1,0,0,0,0,0,0,0};
    float emb2[P2OS_EMBED_DIM] = {0,1,0,0,0,0,0,0};
    char d1[] = "doc1"; char d2[] = "doc2";
    if (p2os_index_put("doc1", emb1, d1, strlen(d1)+1) != 0) { printf("index doc1 fail\n"); return 1; }
    if (p2os_index_put("doc2", emb2, d2, strlen(d2)+1) != 0) { printf("index doc2 fail\n"); return 1; }

    char* out_ids[2]; float out_scores[2];
    float q[P2OS_EMBED_DIM] = {1,0,0,0,0,0,0,0};
    size_t n = p2os_query(q, 2, out_ids, out_scores);
    if (n == 0) { printf("query returned 0\n"); return 1; }
    if (strcmp(out_ids[0], "doc1") != 0) { printf("expected doc1 first, got %s\n", out_ids[0]); return 1; }
    memoryos_shutdown();
    printf("P2OS semantic tests passed\n");
    return 0;
}
