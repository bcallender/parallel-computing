//
// Created by brandon on 1/4/16.
//

#ifndef CLIONTEST_GRAPH_H
#define CLIONTEST_GRAPH_H
#include <stddef.h>
#include <stdlib.h>

#include <string.h>
#include <stdio.h>

typedef struct int_array {
    int* array;
    int length;
    size_t elemSize;
} int_array;

typedef struct matrix {
    unsigned int numNodes;
    int**data;
} matrix;

matrix *createMatrix(unsigned int r, unsigned int c, int initialValue);

int getEntryAt(unsigned int i, unsigned int j, matrix *g);

void setEntryAt(unsigned int i, unsigned int j, matrix *g, int distance);

void setRow(unsigned int r, int_array *columns, matrix *g);
void printMatrix(matrix* g, FILE* outputFile);


#endif //CLIONTEST_GRAPH_H
