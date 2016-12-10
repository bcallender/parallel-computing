//
// Created by brandon on 1/26/16.
//

#include <pthread.h>
#include <signal.h>
#include "firewallImpl.h"
#include "../lib/hashpacketworker.h"

#define MSEC_TO_USEC 1000

volatile bool global_done;


long serialFirewall(int workLoad,
                    int numSources,
                    long mean,
                    unsigned int queueDepth,
                    int initSize,
                    float fractionAdd,
                    float fractionRemove,
                    float hitRate) {
    HashPacketGenerator_t *packetSource = createHashPacketGenerator(fractionAdd, fractionRemove, hitRate, mean);
    CoarseHashTable_t *hashtable = createCoarseHashTable(5, 0.75, 8, noop_init);
    StopWatch_t watch;
    for(int i = 0; i < initSize; i++) {
        HashPacket_t *hp = getAddPacket(packetSource);
        add_coarse(hashtable, hp->key, hp->body);
    }

    startTimer(&watch);


    NoQueuePacketWorker *data = malloc(sizeof(NoQueuePacketWorker));
    data->done.value = false;
    data->fingerprint = 0;
    data->residue = 0;
    data->source = packetSource;
    data->table = hashtable;
    pthread_t pt;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&pt, &attr, (void *) serialWorker, (void *) data);
    useconds_t sleepTime = (unsigned) workLoad * MSEC_TO_USEC;
    usleep(sleepTime);
    data->done.value = true;
    pthread_join(pt, NULL);


    stopTimer(&watch);
    pthread_attr_destroy(&attr);


    printf("%ld\n%ld\n", data->totalPackets, data->residue);
    return data->residue;
}

void catch_alarm(int sig) {
    global_done = true;
    signal(sig, catch_alarm);
}


long queueFirewall(int workLoad,
                   int numSources,
                   long mean,
                   unsigned int queueDepth,
                   int initSize,
                   float fractionAdd,
                   float fractionRemove,
                   float hitRate,
                   int tableType,
                   bool drop) {
    HashPacketGenerator_t *packetSource = createHashPacketGenerator(fractionAdd, fractionRemove, hitRate, mean);
    ConcurrentHashTable_t *cht = NULL;
    CoarseHashTable_t *coarse = NULL;
    int partitionLogSize = 4;
    if(tableType == 1)
        cht = createConcurrentHashTable(5, 1, partitionLogSize, 8);
    else
        coarse = createCoarseHashTable(5, 0.75, 8, mutex_init);
    for(int i = 0; i < initSize; i++) {
        HashPacket_t *hp = getAddPacket(packetSource);
        if(tableType == 0)
            add_coarse(coarse, mangleKey(hp), hp->body);
        else
            add_cht(cht, mangleKey(hp), hp->body);
    }
    StopWatch_t watch;
    long fingerprint = 0;
    long packets_processed = 0;


    QueuePacketWorker **threadData = calloc(numSources, sizeof(QueuePacketWorker *));
    unsigned long tid[numSources];


    for(int l = 0; l < numSources; ++l) {
        QueuePacketWorker *ppw = malloc(sizeof(QueuePacketWorker));
        threadData[l] = ppw;
        threadData[l]->q = createQueue(queueDepth);
        threadData[l]->done.value = false;
        threadData[l]->totalPackets = 0;
        threadData[l]->residue = 0;
        if(tableType == 0)
            threadData[l]->table = coarse;
        else
            threadData[l]->table = cht;


    }
    startTimer(&watch);
    if(drop) {
        for(int i = 0; i < numSources; i++) {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
            pthread_create(&tid[i], &attr, (void *) parallelDropWorker, threadData[i]);
        }
    }
    else if(tableType == 0) {
        for(int i = 0; i < numSources; i++) {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
            pthread_create(&tid[i], &attr, (void *) parallelCoarseLockWorker, threadData[i]);
        }
    } else if(tableType == 1) {
        for(int i = 0; i < numSources; i++) {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
            pthread_create(&tid[i], &attr, (void *) parallelConcurrentWorker, threadData[i]);
        }
    }

    /* Establish a handler for SIGALRM signals. */
    signal(SIGALRM, catch_alarm);

    /* Set an alarm to go off in a little while. */
    alarm(workLoad / 1000);

    /* Check the flag once in a while to see when to quit. */
    while(!global_done) {
        for(int j = 0; j < numSources; j++) {
            if(!is_full(threadData[j]->q)) {
                volatile HashPacket_t *tmp;
                tmp = getRandomPacket(packetSource);
                enq(tmp, threadData[j]->q);
            }
        }
    }


    for(int i = 0; i < numSources; i++) {
        threadData[i]->done.value = true;

    }
    for(int i = 0; i < numSources; i++) {
        pthread_join(tid[i], NULL);
    }

    for(int k = 0; k < numSources; ++k) {
        fingerprint += threadData[k]->residue;
        packets_processed += threadData[k]->totalPackets;
    }
    stopTimer(&watch);

    printf("%ld\n%ld\n", packets_processed, fingerprint);
    return fingerprint;
}
