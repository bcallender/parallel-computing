//
// Created by brandon on 1/25/16.
//

#ifndef PARALLELHW2_QUEUE_H
#define PARALLELHW2_QUEUE_H

#include "../lib/packetsource.h"
#include "locks.h"
#include "../lib/hashgenerator.h"

typedef struct {
    volatile HashPacket_t *packet;
} queue_entry;

typedef struct {
    queue_entry *items;
    int size;
    volatile int head;
    volatile int tail;
} queue;

bool is_full(queue *q);

queue *createQueue(unsigned int size);

int enq(volatile HashPacket_t *packet, queue *q);

volatile HashPacket_t *deq(queue *q);


#endif //PARALLELHW2_QUEUE_H
