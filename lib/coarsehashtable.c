//
// Created by brandon on 3/1/16.
//

#include "coarsehashtable.h"


CoarseHashTable_t *createCoarseHashTable(int logSize, float loadFactor, int expectedBucketSize, lock_creator creator) {
    CoarseHashTable_t *htable = malloc(sizeof(CoarseHashTable_t));
    htable->logSize = logSize;
    htable->loadFactor = loadFactor;
    htable->mask = (1 << logSize) - 1;
    htable->numEntries = 0;
    htable->expectedBucketSize = expectedBucketSize;
    int tableSize = (1 << logSize);
    htable->size = tableSize;
    htable->table = (SerialList_t **) malloc(sizeof(SerialList_t *) * tableSize);
    htable->lock = creator(0);
    for(int i = 0; i < tableSize; i++)
        htable->table[i] = NULL;

    return htable;
}


void resizeIfNecessary_coarse(CoarseHashTable_t *cht, int key) {
    if(cht->numEntries > (cht->loadFactor * cht->expectedBucketSize * cht->size))
        resize_table(cht);
}

void addNoCheck_coarse(CoarseHashTable_t *cht, int key, volatile Packet_t *x) {
    cht->lock->lock(cht->lock);
    int index = key & cht->mask;
    if(cht->table[index] == NULL) {
        cht->table[index] = createSerialListWithItem(key, x);
    }
    else {
        add_list(cht->table[index], key, x);
    }
    cht->numEntries++;

    cht->lock->unlock(cht->lock);
}

void add_coarse(CoarseHashTable_t *cht, int key, volatile Packet_t *x) {
    resizeIfNecessary_coarse(cht, key);
    addNoCheck_coarse(cht, key, x);
}

bool remove_coarse(CoarseHashTable_t *cht, int key) {
    resizeIfNecessary_coarse(cht, key);
    cht->lock->lock(cht->lock);
    if(cht->table[key & cht->mask] != NULL) {
        bool res = remove_list(cht->table[key & cht->mask], key);
        if(res) {
            cht->numEntries--;
        }
        cht->lock->unlock(cht->lock);
        return res;
    }
    else {
        cht->lock->unlock(cht->lock);
        return false;
    }
}


bool contains_coarse(CoarseHashTable_t *cht, int key) {
    cht->lock->lock(cht->lock);
    if(cht->table[key & cht->mask] != NULL) {
        bool res = contains_list(cht->table[key & cht->mask], key);
        cht->lock->unlock(cht->lock);
        return res;
    }
    else {
        cht->lock->unlock(cht->lock);
        return false;
    }

}


void resize_table(CoarseHashTable_t *cht) {
    cht->lock->lock(cht->lock);
    unsigned int newTableSize = (unsigned) cht->size * 2;
    int newMask = (1 << ++cht->logSize) - 1;
    SerialList_t **newTable = calloc(newTableSize, sizeof(SerialList_t *));
    for(int i = 0; i < newTableSize; i++)
        newTable[i] = NULL;

    for(int i = 0; i < cht->size; i++) {
        if(cht->table[i] != NULL) {
            SerialNode *curr = cht->table[i]->head;
            while(curr != NULL) {
                SerialNode *nextItem = curr->next;

                if(newTable[curr->key & newMask] == NULL) {
                    newTable[curr->key & newMask] = createSerialListWithItem(curr->key, curr->value);
                } else {
                    add_list(newTable[curr->key & newMask], curr->key, curr->value);
                }
                free(curr);
                curr = nextItem;
            }
        }
    }
    SerialList_t **temp = cht->table;
    //cht->logSize++;
    cht->size = cht->size * 2;
    cht->mask = (1 << cht->logSize) - 1;
    cht->table = newTable;
    free(temp);
    cht->lock->unlock(cht->lock);
}