//
// Created by brandon on 2/28/16.
//

#ifndef PARALLELHW4_CONCURRENTLIST_H

#define PARALLELHW4_CONCURRENTLIST_H

#include <stdbool.h>
#include "packetsource.h"
#include "../hw4/locks.h"

typedef struct Node {
    int key;
    volatile Packet_t *value;
    struct Node *next;
    struct Lock *lock;
    bool marked;
} Node;

typedef struct {
    int size;
    Node *head;
    Node *tail;
} ConcurrentList_t;

ConcurrentList_t *createConcurrentList();

ConcurrentList_t *createConcurrentListWithItem(int key, volatile Packet_t *value);

Node *getConcurrent(ConcurrentList_t *list, int key);

bool containsConcurrent(ConcurrentList_t *list, int key);

bool removeConcurrent(ConcurrentList_t *list, int key);

bool addConcurrent(ConcurrentList_t *list, int key, volatile Packet_t *value);

bool addConcurrentNoLock(ConcurrentList_t *list, int key, volatile Packet_t *value);

void printConcurrentList(ConcurrentList_t *list);

#endif //PARALLELHW4_CONCURRENTLIST_H
