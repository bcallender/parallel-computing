#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "firewallImpl.h"

#define DEFAULT_NUMBER_OF_ARGS 6

int main(int argc, char *argv[]) {

    int opt;
    int numSources = 0, numPackets = 0, mean = 0, version = 0, parallel = 0;
    short experimentNumber = 0;
    unsigned int queueDepth = 0;
    while((opt = getopt(argc, argv, "n:T:D:W:E:v:p:h")) != -1)
        switch (opt) {
            case 'n':
                numSources = atoi(optarg);
                break;
            case 'T':
                numPackets = atoi(optarg);
                break;
            case 'D':
                queueDepth = (unsigned) atoi(optarg);
                break;
            case 'W':
                mean = atoi(optarg);
                break;
            case 'E':
                experimentNumber = (short) atoi(optarg);
                break;
            case 'v':
                version = atoi(optarg);
                break;
            case 'p':
                parallel = atoi(optarg);
                break;
            default:
                printf("ERROR: Unknown option -%c\n", opt);
                exit(-1);
        }
    switch (parallel) {
        case 0:
            serialFirewall(numPackets, numSources, mean, version, experimentNumber);
            break;
        case 1:
            parallelFirewall(numPackets, numSources, mean, version, experimentNumber, queueDepth);
            break;
        default:
            serialQueueFirewall(numPackets, numSources, mean, version, experimentNumber, queueDepth);
    }


    return 0;
}

