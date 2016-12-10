#ifndef HASHPACKETWORKER_H_
#define HASHPACKETWORKER_H_

#include "paddedprim.h"
#include "hashgenerator.h"
#include "hashtable.h"
#include "../hw4/queue.h"
#include "coarsehashtable.h"
#include "concurrenthashtable.h"

typedef struct {
    PaddedPrimBool_t done;
    HashPacketGenerator_t *source;
    void *table;
    long totalPackets;
    long residue;
    long fingerprint;
} NoQueuePacketWorker;


typedef struct {
    queue *q;
    long residue;
    long totalPackets;
    PaddedPrimBool_t done;
    void *table;
} QueuePacketWorker;


void serialWorker(NoQueuePacketWorker *data);

void parallelCoarseLockWorker(QueuePacketWorker *data);

void parallelConcurrentWorker(QueuePacketWorker *data);

void parallelDropWorker(QueuePacketWorker *data);



#endif /* HASHPACKETWORKER_H_ */
