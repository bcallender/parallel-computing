//
// Created by brandon on 3/1/16.
//

#ifndef PARALLELHW4_COARSEHASHTABLE_H
#define PARALLELHW4_COARSEHASHTABLE_H

#include "../hw4/locks.h"
#include "../hw4/serialLinkedList.h"

typedef struct {
    int logSize;
    int mask;
    float loadFactor;
    int numEntries;
    int size;
    int expectedBucketSize;
    SerialList_t **table;
    struct Lock *lock;
} CoarseHashTable_t;

CoarseHashTable_t *createCoarseHashTable(int logSize, float loadFactor, int expectedBucketSize, lock_creator creator);

void resizeIfNecessary_coarse(CoarseHashTable_t *cht, int key);

void addNoCheck_coarse(CoarseHashTable_t *cht, int key, volatile Packet_t *x);

void add_coarse(CoarseHashTable_t *cht, int key, volatile Packet_t *x);

bool remove_coarse(CoarseHashTable_t *cht, int key);

bool contains_coarse(CoarseHashTable_t *cht, int key);

void resize_table(CoarseHashTable_t *cht);


#endif //PARALLELHW4_COARSEHASHTABLE_H
