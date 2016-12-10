//
// Created by brandon on 2/12/16.
//

#include "locks.h"
#include <stdlib.h>
#include <pthread.h>

void mutex_lock(lock *l) {
    pthread_mutex_lock(l->lockData);
}

void mutex_unlock(lock *l) {
    pthread_mutex_unlock(l->lockData);
}

int mutex_tryLock(lock *l) {
    return !pthread_mutex_trylock(l->lockData);
}

lock *mutex_init(int numThreads) {
    lock *l = malloc(sizeof(lock));
    l->lock = &mutex_lock;
    l->unlock = &mutex_unlock;
    l->tryLock = &mutex_tryLock;
    l->lockData = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(l->lockData, NULL);
    return l;
}

void resize_lock(lock *l) {
    pthread_rwlock_wrlock(l->lockData);
}

void resize_unlock(lock *l) {
    pthread_rwlock_unlock(l->lockData);
}

int resize_tryLock(lock *l) {
    pthread_rwlock_rdlock(l->lockData);
}

lock *resize_init(int numThreads) {
    lock *l = malloc(sizeof(lock));
    l->lock = resize_lock;
    l->unlock = resize_unlock;
    l->tryLock = resize_tryLock;
    l->lockData = malloc(sizeof(pthread_rwlock_t));
    pthread_rwlock_init(l->lockData, NULL);
    return l;
}


void noop_lock(lock *l) {
    return;
}

void noop_unlock(lock *l) {
    return;
}

int noop_tryLock(lock *l) {
    return true;
}

lock *noop_init(int numThreads) {
    struct Lock *l = malloc(sizeof(lock));
    l->lock = noop_lock;
    l->unlock = noop_unlock;
    l->tryLock = noop_tryLock;
    return l;
}





