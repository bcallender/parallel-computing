#include <getopt.h>
#include <stdlib.h>
#include "firewallImpl.h"

#define DEFAULT_NUMBER_OF_ARGS 6

int main(int argc, char *argv[]) {

    int opt;
    int numSources = 0, numPackets = 0, mean = 0, uniform = 0, lockType = 0, strategy = 0;
    short experimentNumber = 0;
    unsigned int queueDepth = 0;
    bool parallel = false;
    while((opt = getopt(argc, argv, "pn:N:D:W:E:u:L:S:h")) != -1)
        switch (opt) {
            case 'p':
                parallel = true;
                break;
            case 'n':
                numSources = atoi(optarg);
                break;
            case 'N':
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
            case 'u':
                uniform = atoi(optarg);
                break;
            case 'L':
                lockType = atoi(optarg);
                break;
            case 'S':
                strategy = atoi(optarg);
                break;
            default:
                printf("ERROR: Unknown option -%c\n", opt);
                exit(-1);
        }
    if(parallel) {
        parallelFirewall(numPackets, numSources, mean, uniform, experimentNumber, queueDepth, lockType, strategy);
    } else {
        serialFirewall(numPackets, numSources, mean, uniform, experimentNumber);
    }

    return 0;
}