//
// Created by brandon on 1/25/16.
//

#include "queue.h"


queue *createQueue(unsigned int size, struct Lock *l) {
    queue *q = malloc(sizeof(queue));
    q->items = calloc(sizeof(queue_entry), size);
    q->size = size;
    q->head = 0;
    q->tail = 0;
    q->q_lock = l;
    return q;
}

int enq(volatile Packet_t *packet, queue *q) {
    if(q->tail - q->head == q->size) return -1;
    queue_entry qe = {packet};
    memcpy(&q->items[q->tail % q->size], &qe, sizeof(qe));
    q->tail++;
    return 0;

}

volatile Packet_t *lockfree_deq(queue *q) {
    if(q->tail - q->head == 0) {
        return NULL;
    }
    queue_entry x = q->items[q->head % q->size];
    q->head++;
    return x.packet;
}

volatile Packet_t *deq(queue *q) {
    if(q->tail - q->head == 0) {
        return NULL;
    }
    q->q_lock->lock(q->q_lock);
    queue_entry x = q->items[q->head % q->size];
    q->head++;
    q->q_lock->unlock(q->q_lock);
    return x.packet;
}

volatile Packet_t *fast_deq(queue *q) {
    if(q->tail - q->head == 0) {
        return NULL;
    }
    if(q->q_lock->tryLock(q->q_lock)) {
        queue_entry x = q->items[q->head % q->size];
        q->head++;
        q->q_lock->unlock(q->q_lock);
        return x.packet;
    } else {
        return NULL;
    }

}