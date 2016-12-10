//
// Created by brandon on 2/16/16.
//

#include <pthread.h>
#include <stdlib.h>
#include "counterImpl.h"
#include "../lib/stopwatch.h"

Counter *create_counter(lock *(*lock_creator)(int), int numThreads) {

    Counter *c = malloc(sizeof(Counter));
    if(lock_creator != NULL) {
        c->l = lock_creator(numThreads);
    }
    c->count = 0;


    return c;
}

Counter *serialCounter(int countUpTo) {
    StopWatch_t stopwatch;
    startTimer(&stopwatch);
    Counter *c = create_counter(NULL, 0);
    for(int i = 0; i < countUpTo; i++) {
        c->count++;
    }
    stopTimer(&stopwatch);
    c->time_taken = getElapsedTime(&stopwatch);
    printf("%ld\n%f\n", c->count, c->time_taken);
    return c;
}

void *parallelCounterWorker(void *tlocal) {
    struct c_local *cl = (struct c_local *) tlocal;
    Counter *c = cl->c;
    long times_called = 0;
    while(times_called < cl->num_iterations) {
        while(!c->l->tryLock(c->l)) { }
        c->count++;
        times_called++;
        c->l->unlock(c->l);
    }
    pthread_exit(NULL);

}

Counter *parallelCounter(int countUpTo, int numThreads, lock *(*lock_creator)(int)) {
    StopWatch_t stopwatch;

    startTimer(&stopwatch);
    Counter *c = create_counter(lock_creator, numThreads);
    pthread_t tid[numThreads];
    for(int i = 0; i < numThreads; i++) {
        struct c_local cl;
        cl.num_iterations = countUpTo / numThreads;
        if(i == numThreads - 1) {
            cl.num_iterations += countUpTo % numThreads;
        }
        cl.c = c;
        pthread_create(&tid[i], NULL, parallelCounterWorker, &cl);
    }
    for(int i = 0; i < numThreads; i++) {
        pthread_join(tid[i], NULL);
    }
    stopTimer(&stopwatch);
    c->time_taken = getElapsedTime(&stopwatch);
    printf("%ld\n%f\n", c->count, c->time_taken);
    return c;


}

