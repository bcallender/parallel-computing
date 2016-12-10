//
// Created by brandon on 1/26/16.
//

#ifndef PARALLELHW2_FIREWALLIMPL_H
#define PARALLELHW2_FIREWALLIMPL_H

#include "../lib/packetsource.h"
#include "../lib/stopwatch.h"
#include "../lib/fingerprint.h"
#include "queue.h"


long serialFirewall(int workLoad,
                    int numSources,
                    long mean,
                    unsigned int queueDepth,
                    int initSize,
                    float fractionAdd,
                    float fractionRemove,
                    float hitRate);

long queueFirewall(int workLoad,
                   int numSources,
                   long mean,
                   unsigned int queueDepth,
                   int initSize,
                   float fractionAdd,
                   float fractionRemove,
                   float hitRate,
                   int tableType,
                   bool drop);


#endif //PARALLELHW2_FIREWALLIMPL_H
