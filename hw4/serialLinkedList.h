//
// Created by brandon on 3/5/16.
//

#ifndef PARALLELHW4_SERIALLINKEDLIST_H
#define PARALLELHW4_SERIALLINKEDLIST_H

#include <stdbool.h>
#include "../lib/packetsource.h"

typedef struct SerialNode {
    int key;
    volatile Packet_t *value;
    struct SerialNode *next;
} SerialNode;

typedef struct {
    int size;
    SerialNode *head;
} SerialList_t;

SerialNode *createSerialNode(int key, volatile Packet_t *value);

SerialList_t *createSerialList();

SerialList_t *createSerialListWithItem(int key, volatile Packet_t *value);

SerialNode *getItem_list(SerialList_t *list, int key);

bool contains_list(SerialList_t *list, int key);

bool remove_list(SerialList_t *list, int key);

bool add_list(SerialList_t *list, int key, volatile Packet_t *value);

void addNoCheck_list(SerialList_t *list, int key, volatile Packet_t *value);

//void print_list(SerialList_t *list);

#endif //PARALLELHW4_SERIALLINKEDLIST_H
