#include "spin_barrier.h"

spin_barrier::spin_barrier(int num_threads) : n(num_threads), counter(0) {
    go = new std::atomic<int>[n];
    for (int i = 0; i < n; i++) {
        go[i].store(0, std::memory_order_relaxed);
    }
}

spin_barrier::~spin_barrier() {
    delete[] go;
}

void spin_barrier::wait(int tid) {
    // 1: Read local sense flag
    int local_go = go[tid].load(std::memory_order_relaxed);

    // 2: Fetch and increment shared counter
    int local_counter = counter.fetch_add(1, std::memory_order_acq_rel);

    // 3: If last thread to arrive
    if (local_counter + 1 == n) {
        // 4: Reset counter
        counter.store(0, std::memory_order_release);
        for (int j = 0; j < n; j++) {
            int current_val = go[j].load(std::memory_order_relaxed);
            go[j].store(1 - current_val, std::memory_order_release);
        }

    } else {
        // 5: Busy wait until last thread flips go[tid]
        while (go[tid].load(std::memory_order_acquire) == local_go) {
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
            #endif
        }
    }
}