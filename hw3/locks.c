//
// Created by brandon on 2/12/16.
//

#include "locks.h"
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

__thread int slot;

void tas_lock(lock *l) {
    tas_lock_data *ldata = (tas_lock_data *) l->lockData;
    while(__sync_lock_test_and_set(&ldata->held, true)) { }

}

void tas_unlock(lock *l) {
    tas_lock_data *ldata = (tas_lock_data *) l->lockData;
    ldata->held = false;
}

int tas_tryLock(lock *l) {
    tas_lock_data *ldata = (tas_lock_data *) l->lockData;
    int res = ldata->held;
    if(res == false) {
        l->lock(l);
        return true;
    } else {
        return false;
    }
}

lock *tas_init(int numThreads) {
    lock *l = malloc(sizeof(lock));
    l->lockData = malloc(sizeof(tas_lock_data));
    tas_lock_data *ld = (tas_lock_data *) l->lockData;
    ld->held = false;
    l->lock = &tas_lock;
    l->unlock = &tas_unlock;
    l->tryLock = &tas_tryLock;
    return l;
}


void backoff_lock(lock *l) {
    backoff_lock_data *ldata = (backoff_lock_data *) l->lockData;
    useconds_t limit = ldata->minDelay;
    while(1 == 1) {
        while(ldata->held == true) { };
        if(!__sync_lock_test_and_set(&ldata->held, true)) {
            return;
        } else {
            srand(time(NULL));
            useconds_t delay = rand() % limit;
            limit = ldata->maxDelay < (2 * limit) ?
                    ldata->maxDelay : (2 * limit);
            usleep(delay);

        }
    }
}

void backoff_unlock(lock *l) {
    backoff_lock_data *ldata = (backoff_lock_data *) l->lockData;
    ldata->held = false;
}

int backoff_tryLock(lock *l) {
    backoff_lock_data *ldata = (backoff_lock_data *) l->lockData;
    int res = ldata->held;
    if(res == false) {
        l->lock(l);
        return true;
    } else {
        return false;
    }
}


lock *backoff_init(int numThreads) {
    lock *l = malloc(sizeof(lock));
    l->lockData = malloc(sizeof(backoff_lock_data));
    l->lock = &backoff_lock;
    l->unlock = &backoff_unlock;
    l->tryLock = &backoff_tryLock;
    backoff_lock_data *ldata = (backoff_lock_data *) l->lockData;
    ldata->maxDelay = 24;
    ldata->minDelay = 1;
    ldata->held = false;
    return l;
}


volatile int alock_tryLock(lock *l) {
    alock_lock_data *ldata = (alock_lock_data *) l->lockData;
    long sl = ldata->tail;
    bool res = ldata->flag[(sl % ldata->size) * ldata->padding_factor];
    if(res) {
        l->lock(l);
        return true;
    } else {
        return false;
    }
}


void alock_lock(lock *l) {
    alock_lock_data *ldata = (alock_lock_data *) l->lockData;
    slot = __sync_fetch_and_add(&ldata->tail, 1) % ldata->size;
    while(!ldata->flag[slot * ldata->padding_factor]) { sched_yield(); };
    ldata->curr = slot;
}

void alock_unlock(lock *l) {
    alock_lock_data *ldata = (alock_lock_data *) l->lockData;
    //int slot = ldata->curr;
    ldata->flag[slot * ldata->padding_factor] = false;
    ldata->flag[((slot + 1) % ldata->size) * ldata->padding_factor] = true;

}

lock *alock_init(int numThreads) {
    lock *l = malloc(sizeof(lock));
    l->lock = &alock_lock;
    l->unlock = &alock_unlock;
    l->tryLock = &alock_tryLock;
    l->lockData = malloc(sizeof(alock_lock_data));
    alock_lock_data *ldata = (alock_lock_data *) l->lockData;
    ldata->size = numThreads;
    ldata->tail = 0;
    ldata->flag = calloc(sizeof(bool), numThreads * 16);
    ldata->flag[0] = true;
    ldata->padding_factor = 16;
    ldata->curr = 0;

    return l;
}


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

qnode* new_node(bool set) {
    qnode* myNode = malloc(sizeof(qnode));
    myNode->locked = set;
    return myNode;
}

void clh_lock(lock *l) {
    clh_lock_data *ldata = (clh_lock_data *) l->lockData;
    qnode *myNode = new_node(true);
    qnode *prev_tail = __sync_lock_test_and_set(&ldata->tail, myNode);
    while(prev_tail->locked) { }
    ldata->current = myNode;
}

void clh_unlock(lock *l) {
    clh_lock_data *ldata = (clh_lock_data *) l->lockData;
    if(ldata->current != NULL) {
        ldata->current->locked = false;

    }
}

int clh_tryLock(lock *l) {
    clh_lock_data *ldata = (clh_lock_data *) l->lockData;
    if(ldata->tail->locked == false) {
        clh_lock(l);
        return true;
    } else {
        return false;
    }
}

lock *clh_init(int numThreads) {
    lock *l = malloc(sizeof(lock));
    l->lock = &clh_lock;
    l->unlock = &clh_unlock;
    l->tryLock = &clh_tryLock;
    l->lockData = malloc(sizeof(clh_lock_data));
    clh_lock_data *ldata = (clh_lock_data *) l->lockData;
    qnode *q = malloc(sizeof(qnode));
    q->locked = false;
    ldata->tail = q;
    return l;
}

lock_creator getLockType(int lockType) {
    switch (lockType) {
        default:
        case 0:
            return tas_init;
        case 1:
            return backoff_init;
        case 2:
            return alock_init;
        case 3:
            return mutex_init;
        case 4:
            return clh_init;
    }
}

