//
// Created by brandon on 3/5/16.
//

#include "serialLinkedList.h"

SerialList_t *createSerialList() {
    SerialList_t *list = malloc(sizeof(SerialList_t));
    list->head = NULL;
    list->size = 0;
    return list;
}


SerialNode *createSerialNode(int key, volatile Packet_t *value) {
    SerialNode *snode = malloc(sizeof(SerialNode));
    snode->value = value;
    snode->key = key;
    snode->next = NULL;
    return snode;
}

SerialList_t *createSerialListWithItem(int key, volatile Packet_t *value) {
    SerialList_t *list = createSerialList();
    SerialNode *snode = createSerialNode(key, value);
    list->head = snode;
    list->size = 1;
    return list;
}

SerialNode *getItem_list(SerialList_t *list, int key) {
    SerialNode *curr = list->head;
    while(curr != NULL) {
        if(curr->key == key)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

bool contains_list(SerialList_t *list, int key) {
    return getItem_list(list, key) != NULL;
}

bool remove_list(SerialList_t *list, int key) {
    SerialNode *prev = list->head;
    if(prev == NULL) {
        return false;
    }
    SerialNode *curr = list->head->next;

    if(prev->key == key) {
        list->head = curr;
        free(prev);
        list->size--;
        return true;
    }
    while(curr != NULL) {
        if(curr->key == key) {
            prev->next = curr->next;
            free(curr);
            list->size--;
            return true;
        }
        prev = curr;
        curr = curr->next;

    }
    return false;
}

bool add_list(SerialList_t *list, int key, volatile Packet_t *value) {
    SerialNode *curr = list->head;
    if(curr == NULL) {
        list->head = createSerialNode(key, value);
        list->size++;
        return true;
    }
    while(curr->next != NULL) {
        if(curr->key == key) {
            curr->value = value;
            return false;

        }
        curr = curr->next;
    }
    if(curr->key == key) {
        curr->value = value;
        return false;
    }
    curr->next = createSerialNode(key, value);
    list->size++;
    return true;
}

void addNoCheck_list(SerialList_t *list, int key, volatile Packet_t *value) {
    add_list(list, key, value);
}

