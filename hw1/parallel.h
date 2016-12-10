//
// Created by brandon on 1/11/16.
//

#include "matrix.h"
#include "stopwatch.h"

#ifndef CLIONTEST_PARALLEL_H
#define CLIONTEST_PARALLEL_H

void parallelFloydWarshall(matrix *graph, matrix *results, unsigned int numThreads);

#endif //CLIONTEST_PARALLEL_H
