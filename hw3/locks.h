//
// Created by brandon on 2/12/16.
//

#ifndef PARALLELHW3_LOCKS_H
#define PARALLELHW3_LOCKS_H

#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>


typedef struct tas_lock_data {
    bool held;
} tas_lock_data;

typedef struct backoff_lock_data {
    useconds_t minDelay, maxDelay;
    volatile bool held;
} backoff_lock_data;

typedef struct alock_lock_data {
    volatile bool *flag;
    int size;
    int padding_factor;
    volatile long tail;
    volatile int curr;
} alock_lock_data;


typedef struct qnode {
    volatile bool locked;
} qnode;

typedef struct clh_lock_data {
    qnode *tail;
    qnode *current;
} clh_lock_data;


typedef struct Lock {
    void *lockData;

    void (*lock)(struct Lock *l);

    void (*unlock)(struct Lock *l);

    int (*tryLock)(struct Lock *l);
} lock;

void tas_lock(lock *l);

void tas_unlock(lock *l);

int tas_tryLock(lock *l);

lock *tas_init(int numThreads);

void backoff_lock(lock *l);

void backoff_unlock(lock *l);

int backoff_tryLock(lock *l);

lock *backoff_init(int numThreads);

void mutex_lock(lock *l);

void mutex_unlock(lock *l);

int mutex_tryLock(lock *l);

lock *mutex_init(int numThreads);

void alock_lock(lock *l);

void alock_unlock(lock *l);

volatile int alock_tryLock(lock *l);

lock *alock_init(int numThreads);

void clh_lock(lock *l);

void clh_unlock(lock *l);

int clh_tryLock(lock *l);

lock *clh_init(int numThreads);

typedef lock *(*lock_creator)(int);

lock_creator getLockType(int lockType);


#endif //PARALLELHW3_LOCKS_H
