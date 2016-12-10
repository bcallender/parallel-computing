//
// Created by brandon on 2/26/16.
//

#include "concurrenthashtable.h"

bool resizeNeeded(Partition *partition);

int firstLevelHash(int crc) {
    const int CRC_POLY = 954680065; // 0x38E74301 - standard CRC30 from CDMA
    const int iterations = 8;
    for(int i = 0; i < iterations; i++) {
        if((crc & 1) > 0)
            crc = (crc >> 1) ^ CRC_POLY;
        else
            crc = crc >> 1;
    }
    return crc;
}

Partition *createPartition(int logSize, float loadFactor, int expectedBucketSize) {
    Partition *htable = malloc(sizeof(Partition));
    htable->logSize = logSize;
    htable->loadFactor = loadFactor;
    htable->mask = (1 << logSize) - 1;
    htable->numEntries = 0;
    htable->expectedBucketSize = expectedBucketSize;
    int tableSize = (1 << logSize);
    htable->size = tableSize;
    htable->table = (ConcurrentList_t **) malloc(sizeof(ConcurrentList_t *) * tableSize);
    htable->lock = resize_init(0);
    for(int i = 0; i < tableSize; i++)
        htable->table[i] = NULL;

    return htable;
}

ConcurrentHashTable_t *createConcurrentHashTable(int logSize, float loadFactor, int partitionLogSize,
                                                 int expectedBucketSize) {
    ConcurrentHashTable_t *cht = malloc(sizeof(ConcurrentHashTable_t));
    cht->partitionLogSize = partitionLogSize;
    cht->partitionMask = (1 << partitionLogSize) - 1;
    int numPartitions = (1 << partitionLogSize);
    cht->partitions = (Partition **) malloc(sizeof(Partition *) * numPartitions);
    for(int i = 0; i < numPartitions; i++)
        cht->partitions[i] = createPartition(logSize, loadFactor, expectedBucketSize);

    return cht;
}


void resizeIfNecessary(Partition *partition) {
    if(resizeNeeded(partition))
        resize_partition(partition);
}

bool resizeNeeded(Partition *partition) {
    return partition->numEntries > (partition->expectedBucketSize * partition->size);
}

void addNoCheck_p(Partition *partition, int key, volatile Packet_t *x) {
    partition->lock->tryLock(partition->lock);
    int pkey = firstLevelHash(key);
    int index = pkey & partition->mask;
    bool newEntry = false;
    if(partition->table[index] == NULL) {
        partition->table[index] = createConcurrentListWithItem(pkey, x);
        newEntry = true;
    }
    else {
        newEntry = addConcurrent(partition->table[index], pkey, x);
    }
    if(newEntry) {
        __sync_fetch_and_add(&partition->numEntries, 1);
    }
    partition->lock->unlock(partition->lock);
}

void add_p(Partition *partition, int key, volatile Packet_t *x) {
    resizeIfNecessary(partition);
    addNoCheck_p(partition, key, x);
}


void add_cht(ConcurrentHashTable_t *table, int key, volatile Packet_t *x) {
    int partitionIndex = partitionForKey(table, key);
    Partition *p = table->partitions[partitionIndex];
    add_p(p, key, x);
}

bool remove_cht(ConcurrentHashTable_t *table, int key) {
    int partitionIndex = partitionForKey(table, key);
    Partition *p = table->partitions[partitionIndex];
    remove_p(p, key);
}

bool remove_p(Partition *partition, int key) {
    int pkey = firstLevelHash(key);
    resizeIfNecessary(partition);
    partition->lock->tryLock(partition->lock);
    const int i = pkey & partition->mask;
    if(partition->table[i] != NULL) {

        bool res = removeConcurrent(partition->table[i], pkey);
        partition->lock->unlock(partition->lock);
        if(res) {
            __sync_fetch_and_add(&partition->numEntries, -1);
        }
        return res;
    }
    else {
        partition->lock->unlock(partition->lock);
        return false;
    }
}

bool contains_cht(ConcurrentHashTable_t *table, int key) {
    int partitionIndex = partitionForKey(table, key);
    Partition *p = table->partitions[partitionIndex];
    return contains_p(p, key);
}

bool contains_p(Partition *partition, int key) {
    int pkey = firstLevelHash(key);
    const int i = pkey & partition->mask;
    partition->lock->tryLock(partition->lock);
    if(partition->table[i] != NULL) {

        bool res = containsConcurrent(partition->table[i], pkey);
        partition->lock->unlock(partition->lock);
        return res;
    }
    else {
        partition->lock->unlock(partition->lock);
        return false;
    }

}


void resize_partition(Partition *partition) {
    partition->lock->lock(partition->lock);
    if(resizeNeeded(partition)) {

        int newTableSize = partition->size * 2;
        ConcurrentList_t **newTable = (ConcurrentList_t **) malloc(sizeof(ConcurrentList_t *) * newTableSize);
        for(int i = 0; i < newTableSize; i++)
            newTable[i] = NULL;

        for(int i = 0; i < partition->size; i++) {
            if(partition->table[i] != NULL) {
                Node *curr = partition->table[i]->head;
                while(curr != NULL) {
                    Node *nextItem = curr->next;
                    if(nextItem != NULL && nextItem->marked == false && nextItem->next != NULL) {
                        int newBucket = nextItem->key & ((2 * partition->mask) + 1);
                        if(newTable[newBucket] == NULL) {
                            newTable[newBucket] = createConcurrentListWithItem(nextItem->key, nextItem->value);
                        } else {
                            addConcurrentNoLock(newTable[newBucket], nextItem->key, nextItem->value);
                        }
                    }
                    curr = nextItem;
                }
            }
        }
        ConcurrentList_t **temp = partition->table;
        partition->logSize++;
        partition->size = partition->size * 2;
        partition->mask = (1 << partition->logSize) - 1;
        partition->table = newTable;
        free(temp);
    }
    partition->lock->unlock(partition->lock);
}

void print_cht(ConcurrentHashTable_t *htable);


int partitionForKey(ConcurrentHashTable_t *table, int key) {
    int part_key = key & table->partitionMask;
    return part_key;
}