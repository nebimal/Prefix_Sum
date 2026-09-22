#ifndef _SPIN_BARRIER_H
#define _SPIN_BARRIER_H

#include <pthread.h>
#include <iostream>
#include <atomic>

class spin_barrier {
    private:
        // Number of threads
        int n;

        // Shared counter register
        std::atomic<int> counter;

        // Shared atomic bit
        std::atomic<int>* go;
    
    public:
        // Constructor initializes N, sets counter to 0, and go to 0
        spin_barrier(int num_Threads);

        // Destructor
        ~spin_barrier();

        // Thread id
        void wait(int tid);

};

#endif
