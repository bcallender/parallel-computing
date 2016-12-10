//
// Created by brandon on 2/16/16.
//

#ifndef PARALLELHW3_COUNTER_H
#define PARALLELHW3_COUNTER_H

#include "locks.h"


typedef struct counter {
    volatile long count;
    lock *l;
    double time_taken;
} Counter;

typedef struct c_local {
    long num_iterations;
    Counter *c;
} c_local;

#endif //PARALLELHW3_COUNTER_H

Counter *create_counter(lock_creator, int);

Counter *parallelCounter(int countUpTo, int numThreads, lock_creator);

Counter *serialCounter(int);

void *parallelCounterWorker(void *);