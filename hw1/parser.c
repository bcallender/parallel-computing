//
// Created by brandon on 1/5/16.
//

#include <stdio.h>
#include <string.h>
#include "parser.h"


int_array* parseLine(char *line, int numVertices) {
    char* delimiter = " ";
    char* reentrant = strdup(line);
    char* saveptr;
    int count = 0;
    int_array* array = malloc(sizeof(int_array));
    array->array = calloc(numVertices, sizeof(int));

    char* token = strtok_r(reentrant, delimiter, &saveptr);
    while(token != NULL) {
        array->array[count] = atoi(token);
        count++;
        token = strtok_r(NULL, delimiter, &saveptr);
    }
    array->length = count;
    array->elemSize = sizeof(int);

    free(reentrant);
    return array;
}

matrix *parseInputFile(char *filename) {
    FILE* openFile = fopen(filename, "r");
    char* line = malloc(8192);
    char *retval = fgets(line, 8192, openFile);
    if(retval == NULL) {
        fprintf(stderr, "Empty file passed to parseInputFile");
        exit(-2);
    }
    int numVertices = atoi(line);
    if(numVertices < 0) {
        fprintf(stderr, "Malformed file passed to parseInputFile: bad vertex count");
        exit(-3);
    }
    numVertices = (unsigned int) numVertices;
    unsigned int row = 0;
    matrix * g = createMatrix(numVertices, numVertices, 0);
    while(fgets(line, 8192, openFile) != NULL && row < numVertices) {
        line[strcspn(line, "\n")] = 0;
        int_array* parsed = parseLine(line, numVertices);
        if(parsed->length != numVertices) {
            fprintf(stderr, "Malformed file passed to parseInputFile: bad matrix");
            exit(-3);
        }
        setRow(row, parsed, g);
        row++;
    }
    free(line);
    fclose(openFile);
    return g;

}
