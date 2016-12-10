//
// Created by brandon on 2/28/16.
//

#include "concurrentList.h"


Node *createSentinel(bool head) {
    Node *n = malloc(sizeof(Node));
    n->next = NULL;
    if(head) {
        n->key = INT32_MAX + 1;
    } else {
        n->key = INT32_MAX;
    }
    n->value = NULL;
    n->lock = mutex_init(0);
    n->marked = false;
    return n;
}

Node *createNode(int key, volatile Packet_t *value) {
    Node *n = malloc(sizeof(Node));
    n->key = key;
    n->value = value;
    n->lock = mutex_init(0);
    n->marked = false;
    return n;
}


ConcurrentList_t *createConcurrentList() {
    ConcurrentList_t *clist = malloc(sizeof(ConcurrentList_t));
    clist->head = createSentinel(true);
    clist->tail = createSentinel(false);
    clist->head->next = clist->tail;
    clist->size = 0;
    return clist;
}

ConcurrentList_t *createConcurrentListWithItem(int key, volatile Packet_t *value) {
    ConcurrentList_t *clist = createConcurrentList();
    addConcurrent(clist, key, value);
    return clist;
}

Node *getConcurrent(ConcurrentList_t *list, int key) {
    Node *curr = list->head->next;
    if(curr->next != NULL) {
        while(curr->key <= key) {
            if(curr->key == key) {
                return curr;
            } else {
                curr = curr->next;
            }
        }
    }
    return NULL;
}

bool containsConcurrent(ConcurrentList_t *list, int key) {
    Node *exists = getConcurrent(list, key);
    return exists != NULL && exists->marked == false;
}

bool validate(Node *pred, Node *curr) {
    return !pred->marked && !curr->marked && pred->next == curr;
}

//inspired by http://cs.brown.edu/~mph/HellerHLMSS05/2005-OPODIS-Lazy.pdf
bool removeConcurrent(ConcurrentList_t *list, int key) {
    while(true) {
        Node *pred = list->head;
        Node *curr = list->head->next;
        while(curr->key < key) {
            pred = curr;
            curr = curr->next;
        }
        pred->lock->lock(pred->lock);
        curr->lock->lock(curr->lock);
        if(validate(pred, curr)) {
            if(curr->key != key) {
                pred->lock->unlock(pred->lock);
                curr->lock->unlock(curr->lock);
                return false;
            } else {

                curr->marked = true;
                pred->next = curr->next;
                __sync_fetch_and_add(&list->size, -1);
                pred->lock->unlock(pred->lock);
                curr->lock->unlock(curr->lock);
                return true;

            }
        }

        pred->lock->unlock(pred->lock);
        curr->lock->unlock(curr->lock);
    }
}

bool addConcurrent(ConcurrentList_t *list, int key, volatile Packet_t *value) {
    while(true) {
        Node *pred = list->head;
        Node *curr = list->head->next;
        while(curr->key < key) {
            pred = curr;
            curr = curr->next;
        }
        pred->lock->lock(pred->lock);
        curr->lock->lock(curr->lock);
        if(validate(pred, curr)) {
            if(curr->key == key) {
                curr->value = value;
                pred->lock->unlock(pred->lock);
                curr->lock->unlock(curr->lock);
                return false;
            } else {
                Node *n = createNode(key, value);
                n->next = curr;

                pred->next = n;
                pred->lock->unlock(pred->lock);
                curr->lock->unlock(curr->lock);
                __sync_fetch_and_add(&list->size, 1);
                return true;

            }
        }

        pred->lock->unlock(pred->lock);
        curr->lock->unlock(curr->lock);
    }

}

bool addConcurrentNoLock(ConcurrentList_t *list, int key, volatile Packet_t *value) {
    Node *pred = list->head;
    Node *curr = list->head->next;
    while(curr->key < key) {
        pred = curr;
        curr = curr->next;
    }
    if(validate(pred, curr)) {
        if(curr->key == key) {
            curr->value = value;
            return false;
        } else {
            Node *n = createNode(key, value);
            n->next = curr;
            pred->next = n;
            __sync_fetch_and_add(&list->size, 1);
            return true;

        }
    }
    return false;
}


void printConcurrentList(ConcurrentList_t *list) {
    Node *curr = list->head;
    printf("HEAD addr:%p\t", curr);
    curr = curr->next;
    while(curr->key != NULL) {
        printf("addr:%p\tkey:%d\tvalue:%p\tmarked:%d\n", curr, curr->key, curr->value, curr->marked);
        curr = curr->next;
    }
    printf("TAIL addr:%p\t", curr);

}