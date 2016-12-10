//
// Created by brandon on 12/22/15.
//


#include "main.h"
#include "serial.h"
#include "parallel.h"

int main(int argc, char *argv[]) {
    int opt;
    char* inputFilename = "graphInput";
    char* outputFilename = "graphOutput";
    int version = 0;
    int numThreads = 1;

    while ((opt = getopt(argc, argv, "i:o:spt:h")) != -1)
        switch (opt)
        {
            case 'i':
                inputFilename = strdup(optarg);
                break;
            case 'o':
                outputFilename = strdup(optarg);
                break;
            case 's':
                version = 0;
                break;
            case 'p':
                version = 1;
                break;
            case 't':
                numThreads = atoi(optarg);
                break;
            default:
                printf("ERROR: Unknown option -%c\n", opt);
                exit(-1);
        }



    matrix *graph = parseInputFile(inputFilename);
    matrix *results = createMatrix(graph->numNodes, graph->numNodes, 0);
    if(version == 0){
        serialFloydWarshall(graph, results);
    } else {
        parallelFloydWarshall(graph, results, numThreads);
    }

    FILE* out = fopen(outputFilename, "w");
    printMatrix(results, out);
    return 0;
}


