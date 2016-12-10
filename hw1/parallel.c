//
// Created by brandon on 1/11/16.
//

#include <pthread.h>
#include "parallel.h"


typedef struct syncopation {
    matrix *results;
    pthread_barrier_t *barrier;
    int start;
    int finish;
} synchro;

void *threadedFloydWarshall(void *vargp) {

    synchro *sync = (synchro *) vargp;
    matrix *results = sync->results;

    for(int k = 0; k < results->numNodes; k++) {
        for(int i = sync->start; i < sync->finish; i++) {
            for(int j = 0; j < results->numNodes; j++) {
                int throughDistance = getEntryAt(i, k, results) + getEntryAt(k, j, results);
                if(getEntryAt(i, j, results) > throughDistance) {
                    setEntryAt(i, j, results, throughDistance);
                }
            }
        }
        pthread_barrier_wait(sync->barrier);
    }
    pthread_exit(NULL);
}


void parallelFloydWarshall(matrix *graph, matrix *results, unsigned int numThreads) {
    for(int u = 0; u < results->numNodes; u++) {
        for(int v = 0; v < results->numNodes; v++) { //for each edge
            if(u == v) {
                setEntryAt(u, v, results, 0);
            }
            setEntryAt(u, v, results, getEntryAt(u, v, graph));
        }
    }
    StopWatch_t *stopwatch = malloc(sizeof(StopWatch_t));
    if(numThreads > results->numNodes) numThreads = (unsigned int) results->numNodes;
    startTimer(stopwatch);
    pthread_t tid[numThreads];
    synchro threadData[numThreads];
    pthread_barrier_t *barrier = malloc(__SIZEOF_PTHREAD_BARRIER_T);
    pthread_barrier_init(barrier, NULL, numThreads);
    const int totalElements = results->numNodes;
    int tasksPer = (totalElements + numThreads - 1) / numThreads;
    for(int i = 0; i < numThreads; i++) {
        threadData[i].start = i * tasksPer;
        threadData[i].finish = (i + 1) * tasksPer;
        threadData[i].results = results;
        threadData[i].barrier = barrier;
    }
    threadData[numThreads - 1].finish = totalElements;


    for(int i = 0; i < numThreads; i++) {
        pthread_create(&tid[i], NULL, threadedFloydWarshall, &threadData[i]);
    }


    for(int i = 0; i < numThreads; i++) {
        pthread_join(tid[i], NULL);
    }


    stopTimer(stopwatch);
    printf("%f", getElapsedTime(stopwatch));
    free(stopwatch);


}

