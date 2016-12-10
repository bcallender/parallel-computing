//
// Created by brandon on 1/11/16.
//

#include "serial.h"

void serialFloydWarshall(matrix *graph, matrix *results) {
    for(int u = 0; u < results->numNodes; u++) {
        for(int v = 0; v < results->numNodes; v++) { //for each edge
            setEntryAt(u,v, results, getEntryAt(u, v, graph));
        }
    }
    StopWatch_t *stopwatch = malloc(sizeof(StopWatch_t));
    startTimer(stopwatch);

    for(int k = 0; k < results->numNodes; k++) {
        for(int i = 0; i < results->numNodes; i++) {
            for(int j = 0; j < results->numNodes; j++) {
                int throughDistance = getEntryAt(i, k, results) + getEntryAt(k, j, results);
                if(getEntryAt(i, j, results) > throughDistance) {
                    setEntryAt(i, j, results, throughDistance);
                }
            }
        }
    }
    stopTimer(stopwatch);
    printf("%f", getElapsedTime(stopwatch));


}