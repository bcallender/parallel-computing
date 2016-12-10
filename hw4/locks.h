//
// Created by brandon on 2/12/16.
//

#ifndef PARALLELHW3_LOCKS_H
#define PARALLELHW3_LOCKS_H

#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <bits/pthreadtypes.h>


typedef struct Lock {
    void *lockData;

    void (*lock)(struct Lock *l);

    void (*unlock)(struct Lock *l);

    int (*tryLock)(struct Lock *l);
} lock;


void mutex_lock(lock *l);

void mutex_unlock(lock *l);

int mutex_tryLock(lock *l);

lock *mutex_init(int numThreads);


void resize_lock(lock *l);

void resize_unlock(lock *l);

int resize_tryLock(lock *l);

lock *resize_init(int numThreads);

void noop_lock(lock *l);

void noop_unlock(lock *l);

int noop_tryLock(lock *l);

lock *noop_init(int numThreads);

typedef lock *(*lock_creator)(int);


#endif //PARALLELHW3_LOCKS_H
