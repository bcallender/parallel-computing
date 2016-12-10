//
// Created by brandon on 2/26/16.
//

#ifndef PARALLELHW4_CONCURRENTHASHTABLE_H
#define PARALLELHW4_CONCURRENTHASHTABLE_H

#include <stdbool.h>
#include "packetsource.h"
#include "concurrentList.h"

typedef struct {
    int logSize;
    int mask;
    float loadFactor;
    int numEntries;
    int size;
    int expectedBucketSize;
    ConcurrentList_t **table;
    struct Lock *lock;
} Partition;


typedef struct {
    int partitionLogSize;
    int partitionMask;
    Partition **partitions;

} ConcurrentHashTable_t;

Partition *createPartition(int logSize, float loadFactor, int expectedBucketSize);

ConcurrentHashTable_t *createConcurrentHashTable(int logSize, float loadFactor, int partitionLogSize, int);

void resizeIfNecessary(Partition *);

void addNoCheck_p(Partition *, int key, volatile Packet_t *x);

void add_p(Partition *, int key, volatile Packet_t *x);

void add_cht(ConcurrentHashTable_t *, int key, volatile Packet_t *x);

bool remove_cht(ConcurrentHashTable_t *, int key);

bool remove_p(Partition *, int key);

bool contains_cht(ConcurrentHashTable_t *, int key);

bool contains_p(Partition *, int key);

void resize_partition(Partition *);

void print_cht(ConcurrentHashTable_t *htable);

int partitionForKey(ConcurrentHashTable_t *, int key);

int firstLevelHash(int crc);

#endif //PARALLELHW4_CONCURRENTHASHTABLE_H
