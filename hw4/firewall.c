#include <getopt.h>
#include <stdlib.h>
#include "firewallImpl.h"

#define DEFAULT_NUMBER_OF_ARGS 6

int main(int argc, char *argv[]) {

    int opt;
    int numSources = 1, numMilliseconds = 10000, mean = 200, initSize = 10, version = 1;
    float fractionAdd = 0.1, fractionRemove = 0.1, hitRate = 0.5;
    unsigned int queueDepth = 16;
    int tableType = 1;
    while((opt = getopt(argc, argv, "v:n:M:W:i:a:r:H:T:")) != -1)
        switch (opt) {
            case 'v':
                version = atoi(optarg);
                break;
            case 'n':
                numSources = atoi(optarg);
                break;
            case 'M':
                numMilliseconds = atoi(optarg);
                break;
            case 'W':
                mean = atoi(optarg);
                break;
            case 'i':
                initSize = atoi(optarg);
                break;
            case 'a':
                fractionAdd = (float) atof(optarg);
                break;
            case 'r':
                fractionRemove = (float) atof(optarg);
                break;
            case 'T':
                tableType = atoi(optarg);
                break;
            case 'H':
                hitRate = (float) atof(optarg);
                break;
            default:
                printf("ERROR: Unknown option -%c\n", opt);
                exit(-1);
        }
    if(version == 0) {
        serialFirewall(numMilliseconds, numSources, mean,
                       queueDepth, initSize, fractionAdd, fractionRemove, hitRate);
    } else if(version == 1) {
        queueFirewall(numMilliseconds, numSources, mean, queueDepth,
                      initSize, fractionAdd, fractionRemove, hitRate, tableType, false);

    } else {
        queueFirewall(numMilliseconds, numSources, mean, queueDepth,
                      initSize, fractionAdd, fractionRemove, hitRate, tableType, true);
    }

    return 0;
}