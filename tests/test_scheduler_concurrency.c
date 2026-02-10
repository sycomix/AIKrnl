#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "../include/memoryos.h"
#include "../src/scheduler/scheduler.h"

#define NUM_ALLOCS 256
#define NUM_ALLOC_THREADS 2
#define NUM_TOUCH_THREADS 4
#define NUM_SCHED_THREADS 4
#define RUN_TIME_SECS 5

static void* allocs[NUM_ALLOCS];
static pthread_mutex_t allocs_lock;
static volatile int stop_flag = 0;

static int rand_range(int lo, int hi) { return lo + rand() % (hi - lo + 1); }

static void* allocator_thread(void* arg) {
    (void)arg;
    while (!stop_flag) {
        int i = rand_range(0, NUM_ALLOCS - 1);
        size_t s = rand_range(16, 1024);
        void* p = memoryos_alloc(s);
        pthread_mutex_lock(&allocs_lock);
        if (allocs[i]) memoryos_free(allocs[i]);
        allocs[i] = p;
        pthread_mutex_unlock(&allocs_lock);
        usleep(1000);
    }
    return NULL;
}

static void* toucher_thread(void* arg) {
    (void)arg;
    while (!stop_flag) {
        int i = rand_range(0, NUM_ALLOCS - 1);
        pthread_mutex_lock(&allocs_lock);
        void* p = allocs[i];
        pthread_mutex_unlock(&allocs_lock);
        if (p) memoryos_touch(p);
        usleep(500);
    }
    return NULL;
}

static void* scheduler_thread(void* arg) {
    (void)arg;
    while (!stop_flag) {
        // use a low threshold to trigger on items promoted by touchers
        scheduler_run_once(10);
        usleep(2000);
    }
    return NULL;
}

int main(void) {
    srand((unsigned)time(NULL));
    if (memoryos_init(128 * 1024) != 0) { printf("memoryos_init failed\n"); return 1; }
    pthread_mutex_init(&allocs_lock, NULL);

    // pre-populate
    for (int i = 0; i < NUM_ALLOCS; ++i) allocs[i] = memoryos_alloc(128);

    pthread_t allocers[NUM_ALLOC_THREADS];
    pthread_t touchers[NUM_TOUCH_THREADS];
    pthread_t scheds[NUM_SCHED_THREADS];

    for (int i = 0; i < NUM_ALLOC_THREADS; ++i) pthread_create(&allocers[i], NULL, allocator_thread, NULL);
    for (int i = 0; i < NUM_TOUCH_THREADS; ++i) pthread_create(&touchers[i], NULL, toucher_thread, NULL);
    for (int i = 0; i < NUM_SCHED_THREADS; ++i) pthread_create(&scheds[i], NULL, scheduler_thread, NULL);

    sleep(RUN_TIME_SECS);
    stop_flag = 1;

    for (int i = 0; i < NUM_ALLOC_THREADS; ++i) pthread_join(allocers[i], NULL);
    for (int i = 0; i < NUM_TOUCH_THREADS; ++i) pthread_join(touchers[i], NULL);
    for (int i = 0; i < NUM_SCHED_THREADS; ++i) pthread_join(scheds[i], NULL);

    // cleanup
    pthread_mutex_lock(&allocs_lock);
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (allocs[i]) {
            memoryos_free(allocs[i]);
            allocs[i] = NULL;
        }
    }
    pthread_mutex_unlock(&allocs_lock);

    pthread_mutex_destroy(&allocs_lock);
    memoryos_shutdown();

    printf("Scheduler concurrency stress test passed\n");
    return 0;
}
