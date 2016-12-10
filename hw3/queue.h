//
// Created by brandon on 1/25/16.
//

#ifndef PARALLELHW2_QUEUE_H
#define PARALLELHW2_QUEUE_H

#include "../lib/packetsource.h"
#include "locks.h"

typedef struct {
    volatile Packet_t *packet;
} queue_entry;

typedef struct {
    queue_entry *items;
    int size;
    volatile int head;
    volatile int tail;
    struct Lock* q_lock;
} queue;

queue *createQueue(unsigned int size, struct Lock *l);

volatile Packet_t *lockfree_deq(queue *q);

int enq(volatile Packet_t *packet, queue *q);

volatile Packet_t *deq(queue *q);

volatile Packet_t *fast_deq(queue *q);


#endif //PARALLELHW2_QUEUE_H
