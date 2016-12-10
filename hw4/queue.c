//
// Created by brandon on 1/25/16.
//

#include "queue.h"


queue *createQueue(unsigned int size) {
    queue *q = malloc(sizeof(queue));
    q->items = calloc(sizeof(queue_entry), size);
    q->size = size;
    q->head = 0;
    q->tail = 0;
    return q;
}

bool is_full(queue *q) {
    return (q->tail - q->head == q->size);
}

int enq(volatile HashPacket_t *packet, queue *q) {
    if(q->tail - q->head == q->size) return -1;
    queue_entry qe = {packet};
    memcpy(&q->items[q->tail % q->size], &qe, sizeof(qe));
    q->tail++;
    return 0;

}

volatile HashPacket_t *deq(queue *q) {
    if(q->tail - q->head == 0) {
        return NULL;
    }
    queue_entry x = q->items[q->head % q->size];
    q->head++;
    return x.packet;
}

