//
// Created by brandon on 1/26/16.
//

#ifndef PARALLELHW2_FIREWALLIMPL_H
#define PARALLELHW2_FIREWALLIMPL_H

#include "../lib/packetsource.h"
#include "../lib/stopwatch.h"
#include "../lib/fingerprint.h"
#include "queue.h"

typedef struct {
    queue *q;
    long fingerprint;
    volatile int done;
} synchro;

long serialFirewall(const int,
                    const int,
                    const long,
                    const int,
                    const short);

long serialQueueFirewall(int numPackets,
                         int numSources,
                         long mean,
                         int version,
                         short experimentNumber,
                         unsigned int queueDepth);

long parallelFirewall(int numPackets,
                      int numSources,
                      long mean,
                      int version,
                      short experimentNumber,
                      unsigned int queueDepth);

void *parallelFirewallWorker(void *varargs);


#endif //PARALLELHW2_FIREWALLIMPL_H
