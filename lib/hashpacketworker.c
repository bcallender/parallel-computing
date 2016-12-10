#include <pthread.h>
#include "hashpacketworker.h"
#include "fingerprint.h"

void serialWorker(NoQueuePacketWorker *data) {
    HashPacket_t *pkt;

    while(!data->done.value) {

        pkt = getRandomPacket(data->source);
        data->residue += getFingerprint(pkt->body->iterations, pkt->body->seed);

        switch (pkt->type) {
            case Add:
                add_coarse(data->table, mangleKey(pkt), pkt->body);
                break;
            case Remove:
                remove_coarse(data->table, mangleKey(pkt));
                break;
            case Contains:
                contains_coarse(data->table, mangleKey(pkt));
                break;
        }
        data->totalPackets++;
        pkt = NULL;
    }
}



void parallelCoarseLockWorker(QueuePacketWorker *data) {
    CoarseHashTable_t *table = (CoarseHashTable_t *) data->table;
    while(!data->done.value) {
        volatile HashPacket_t *pkt = deq(data->q);
        if(pkt != NULL) {
            data->residue += getFingerprint(pkt->body->iterations, pkt->body->seed);
            switch (pkt->type) {
                case Add:
                    add_coarse(table, pkt->key, pkt->body);
                    break;
                case Remove:
                    remove_coarse(table, pkt->key);
                    break;
                case Contains:
                    contains_coarse(table, pkt->key);
                    break;
            }
            data->totalPackets++;
            pkt = NULL;
        }


    }
    pthread_exit(NULL);
}

void parallelDropWorker(QueuePacketWorker *data) {

    while(!data->done.value) {
        volatile HashPacket_t *pkt = deq(data->q);
        if(pkt != NULL) {
            data->totalPackets++;
        }


    }
    pthread_exit(NULL);
}

void parallelConcurrentWorker(QueuePacketWorker *data) {
    ConcurrentHashTable_t *table = (ConcurrentHashTable_t *) data->table;
    while(!data->done.value) {
        volatile HashPacket_t *pkt = deq(data->q);
        if(pkt != NULL) {
            data->residue += getFingerprint(pkt->body->iterations, pkt->body->seed);
            switch (pkt->type) {
                case Add:
                    add_cht(table, pkt->key, pkt->body);
                    break;
                case Remove:
                    remove_cht(table, pkt->key);
                    break;
                case Contains:
                    contains_cht(table, pkt->key);
                    break;
            }
            data->totalPackets++;
            pkt = NULL;
        }
    }
    pthread_exit(NULL);
}

