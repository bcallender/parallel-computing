//
// Created by brandon on 1/25/16.
//

#ifndef PARALLELHW2_QUEUE_H
#define PARALLELHW2_QUEUE_H

#include "../lib/packetsource.h"

typedef struct {
    volatile Packet_t *packet;
} queue_entry;

typedef struct {
    queue_entry *items;
    int size;
    volatile int head;
    volatile int tail;
} queue;

queue *createQueue(unsigned int size);

int enq(volatile Packet_t *packet, queue *q);

volatile Packet_t *deq(queue *q);


#endif //PARALLELHW2_QUEUE_H
