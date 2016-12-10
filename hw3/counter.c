//
// Created by brandon on 2/18/16.
//

#include <stdlib.h>
#include "counterImpl.h"

int main(int argc, char *argv[]) {

    int opt;
    int numThreads = 0, lockType = 0, total = 10000;
    bool parallel = false;
    while((opt = getopt(argc, argv, "pN:L:T:")) != -1)
        switch (opt) {
            case 'p':
                parallel = true;
                break;
            case 'N':
                numThreads = atoi(optarg);
                break;
            case 'L':
                lockType = atoi(optarg);
                break;
            case 'T':
                total = atoi(optarg);
                break;
            default:
                printf("ERROR: Unknown option -%c\n", opt);
                exit(-1);
        }
    if(parallel) {
        parallelCounter(total, numThreads, getLockType(lockType));
    } else {
        serialCounter(total);
    }

    return 0;
}