#pragma once 
// ==========================================
// DYNAMIC GLOBAL OBJECT POOL
// ==========================================
#define MAX_CACHED_MFS 384

#define _GNU_SOURCE
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif

// Forward declaration assuming this struct is defined elsewhere
// struct hc_matchfinder; 

static struct hc_matchfinder* g_mf_pool[MAX_CACHED_MFS];
static bool g_pool_atexit_registered = false;

// Cleans up the pool when the program terminates
static void WRC_cleanup_pool(void) {
    for (int i = 0; i < MAX_CACHED_MFS; i++) {
        struct hc_matchfinder* cached = __atomic_exchange_n(&g_mf_pool[i], NULL, __ATOMIC_SEQ_CST);
        if (cached != NULL) {
            munmap(cached, sizeof(struct hc_matchfinder));
        }
    }
}

static struct hc_matchfinder* WRC_get_matchfinder(void) {
    // Register cleanup once (Thread-safe check)
    if (!__atomic_test_and_set(&g_pool_atexit_registered, __ATOMIC_SEQ_CST)) {
        atexit(WRC_cleanup_pool);
    }

    // Try to pop from the lock-free pool
    for (int i = 0; i < MAX_CACHED_MFS; i++) {
        struct hc_matchfinder* cached = __atomic_exchange_n(&g_mf_pool[i], NULL, __ATOMIC_SEQ_CST);
        if (cached != NULL) {
            cached->initialized = 0;
            return cached; 
        }
    }

    // Pool empty: allocate new using mmap
    size_t size = sizeof(struct hc_matchfinder);
    
    // 1. Use mmap instead of malloc. mmap returns page-aligned memory.
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (ptr == MAP_FAILED) {
        return NULL;
    }

    // 2. Advise the kernel to use Huge Pages for this specific range.
    madvise(ptr, size, MADV_HUGEPAGE);

    struct hc_matchfinder* mf = (struct hc_matchfinder*)ptr;
    mf->initialized = 0;

    return mf;
}

// Pushes a matchfinder back to the pool. 
void WRC_release_matchfinder(void* mf_ptr) {
    struct hc_matchfinder* mf = (struct hc_matchfinder*)mf_ptr;
    if (mf == NULL) return;
    mf->initialized = 0;

    for (int i = 0; i < MAX_CACHED_MFS; i++) {
        struct hc_matchfinder* expected = NULL;
        // If g_mf_pool[i] is NULL, swap it with mf
        if (__atomic_compare_exchange_n(&g_mf_pool[i], &expected, mf, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            return; 
        }
    }

    // PROPER DEALLOCATION: Pool is full, return memory to OS
    munmap(mf, sizeof(struct hc_matchfinder));
}
