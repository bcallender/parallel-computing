#include "hashtable.h"

SerialHashTable_t *createSerialHashTable(int logSize, int maxBucketSize) {
    SerialHashTable_t *htable = (SerialHashTable_t *) malloc(sizeof(SerialHashTable_t));
    htable->logSize = logSize;
    htable->maxBucketSize = maxBucketSize;
    htable->mask = (1 << logSize) - 1;
    int tableSize = (1 << logSize);
    htable->size = tableSize;
    htable->table = (SerialList_t **) malloc(sizeof(SerialList_t *) * tableSize);
    for(int i = 0; i < tableSize; i++)
        htable->table[i] = NULL;

    return htable;
}

SerialHashTable_t *createSerialHashTableWithLock(int logSize, int maxBucketSize) {
    SerialHashTable_t *htable = (SerialHashTable_t *) malloc(sizeof(SerialHashTable_t));
    htable->logSize = logSize;
    htable->maxBucketSize = maxBucketSize;
    htable->mask = (1 << logSize) - 1;
    int tableSize = (1 << logSize);
    htable->size = tableSize;
    htable->table = (SerialList_t **) malloc(sizeof(SerialList_t *) * tableSize);
    for(int i = 0; i < tableSize; i++)
        htable->table[i] = NULL;
    htable->lock = mutex_init(0);
    return htable;
}


void resizeIfNecessary_ht(SerialHashTable_t *htable, int key) {
    while(htable->table[key & htable->mask] != NULL
          && htable->table[key & htable->mask]->size >= htable->maxBucketSize)
        resize_ht(htable);
}

void addNoCheck_ht(SerialHashTable_t *htable, int key, volatile Packet_t *x) {
    int index = key & htable->mask;
    if(htable->table[index] == NULL)
        htable->table[index] = createSerialListWithItem(key, x);
    else
        addNoCheck_list(htable->table[index], key, x);
}

void add_ht(SerialHashTable_t *htable, int key, volatile Packet_t *x) {
    resizeIfNecessary_ht(htable, key);
    addNoCheck_ht(htable, key, x);
}

bool remove_ht(SerialHashTable_t *htable, int key) {
    resizeIfNecessary_ht(htable, key);
    if(htable->table[key & htable->mask] != NULL)
        return remove_list(htable->table[key & htable->mask], key);
    else
        return false;

}

bool contains_ht(SerialHashTable_t *htable, int key) {
    int myMask = htable->size - 1;
    if(htable->table[key & myMask] != NULL)
        return contains_list(htable->table[key & myMask], key);
    else
        return false;
}

void resize_ht(SerialHashTable_t *htable) {

    int newTableSize = htable->size * 2;
    SerialList_t **newTable = (SerialList_t **) malloc(sizeof(SerialList_t *) * newTableSize);
    for(int i = 0; i < newTableSize; i++)
        newTable[i] = NULL;

    for(int i = 0; i < htable->size; i++) {
        if(htable->table[i] != NULL) {
            SerialNode *curr = htable->table[i]->head;
            while(curr != NULL) {
                SerialNode *nextItem = curr->next;
                if(newTable[curr->key & ((2 * htable->mask) + 1)] == NULL) {
                    newTable[curr->key & ((2 * htable->mask) + 1)] = createSerialList();
                    curr->next = NULL;
                    newTable[curr->key & ((2 * htable->mask) + 1)]->head = curr;
                } else {
                    curr->next = newTable[curr->key & ((2 * htable->mask) + 1)]->head;
                    newTable[curr->key & ((2 * htable->mask) + 1)]->head = curr;
                }
                curr = nextItem;
            }
        }
    }
    SerialList_t **temp = htable->table;
    htable->logSize++;
    htable->size = htable->size * 2;
    htable->mask = (1 << htable->logSize) - 1;
    htable->table = newTable;
    free(temp);
}
