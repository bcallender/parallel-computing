#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "../hw4/serialLinkedList.h"
#include "../hw4/locks.h"
#include <stdbool.h>

typedef struct {
    int logSize;
    int mask;
    int maxBucketSize;
    int size;
    SerialList_t **table;
    struct Lock *lock;
} SerialHashTable_t;

SerialHashTable_t *createSerialHashTable(int logSize, int maxBucketSize);

SerialHashTable_t *createSerialHashTableWithLock(int logSize, int maxBucketSize);

void resizeIfNecessary_ht(SerialHashTable_t *htable, int key);

void addNoCheck_ht(SerialHashTable_t *htable, int key, volatile Packet_t *x);

void add_ht(SerialHashTable_t *htable, int key, volatile Packet_t *x);

bool remove_ht(SerialHashTable_t *htable, int key);

bool contains_ht(SerialHashTable_t *htable, int key);

void resize_ht(SerialHashTable_t *htable);



#endif /* HASHTABLE_H_ */
