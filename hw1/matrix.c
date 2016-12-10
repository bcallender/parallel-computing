//
// Created by brandon on 1/4/16.
//


#include "matrix.h"


matrix *createMatrix(unsigned int r, unsigned int c, int initialValue) {
    int** mat = malloc(r * sizeof(int*));

    for(int i=0; i<r; i++){
        int *adj = (int *)calloc(c, sizeof(int));
        for(int j = 0; j<c; j++) {
            adj[j] = initialValue;
        }
        mat[i] = adj;
    }
    matrix * g = malloc(sizeof(matrix));
    g->data = mat;
    g->numNodes = r;
    return g;
}

int getEntryAt(unsigned int i, unsigned int j, matrix *g) {
    if(i < g->numNodes && j < g->numNodes) {
        int edgeLength = g->data[i][j];
        return edgeLength;
    } else {
        fprintf(stderr, "Received request for out of bounds point (%d, %d)", i, j);
        exit(-1);
    }

}

void setEntryAt(unsigned int i, unsigned int j, matrix *g, int distance) {
    g->data[i][j] = distance;
}

void setRow(unsigned int r, int_array *columns, matrix *g) {
    memcpy(g->data[r], columns->array, columns->length*columns->elemSize);

}

void printMatrix(matrix* m, FILE* outputFile) {
    fprintf(outputFile, "%d\n", m->numNodes);
    for(int u = 0; u < m->numNodes; u++) {
        for(int v = 0; v < m->numNodes; v++) { //for each edge
           fprintf(outputFile, "%d ", m->data[u][v]);
        }
        fprintf(outputFile, "\n");
    }
}
