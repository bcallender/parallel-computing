//
// Created by brandon on 1/26/16.
//

#include <pthread.h>
#include "firewallImpl.h"


long serialFirewall(int numPackets,
                    int numSources,
                    long mean,
                    int version,
                    short experimentNumber) {
    PacketSource_t *packetSource = createPacketSource(mean, numSources, experimentNumber);
    StopWatch_t watch;
    long fingerprint = 0;

    startTimer(&watch);
    for(int i = 0; i < numSources; i++) {
        for(int j = 0; j < numPackets; j++) {
            volatile Packet_t *tmp;
            switch (version) {
                case 0:
                    tmp = getUniformPacket(packetSource, i);
                    break;
                default:
                    tmp = getExponentialPacket(packetSource, i);
                    break;
            }
            fingerprint += getFingerprint(tmp->iterations, tmp->seed);
        }
    }
    stopTimer(&watch);

    printf("%f\n%ld\n", getElapsedTime(&watch), fingerprint);
    return fingerprint;
}


void *lock_free_worker(void *worker_args) {
    synchro *input = (synchro *) worker_args;
    while(input->done == 0) {
        volatile Packet_t *deq_tmp = lockfree_deq(input->queues[input->queue_index]);
        while(deq_tmp == NULL && input->done == 0) {
            deq_tmp = lockfree_deq(input->queues[input->queue_index]);
        }

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);

        }

    }
    volatile Packet_t *deq_tmp = lockfree_deq(input->queues[input->queue_index]);
    while(deq_tmp != NULL) {

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);
        }

        deq_tmp = lockfree_deq(input->queues[input->queue_index]);

    }
    pthread_exit(NULL);
}

void *home_queue_worker(void *worker_args) {
    synchro *input = (synchro *) worker_args;
    while(input->done == 0) {
        volatile Packet_t *deq_tmp = deq(input->queues[input->queue_index]);
        while(deq_tmp == NULL && input->done == 0) {
            deq_tmp = deq(input->queues[input->queue_index]);
        }

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);

        }

    }
    volatile Packet_t *deq_tmp = deq(input->queues[input->queue_index]);
    while(deq_tmp != NULL) {

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);
        }

        deq_tmp = deq(input->queues[input->queue_index]);

    }
    pthread_exit(NULL);
}

void *random_queue_worker(void *worker_args) {
    synchro *input = (synchro *) worker_args;
    srand(time(NULL));
    int chosen_queue = (int) random() % input->num_queues;
    while(input->done == 0) {
        volatile Packet_t *deq_tmp = deq(input->queues[chosen_queue]);
        while(deq_tmp == NULL && input->done == 0) {
            chosen_queue = (int) random() % input->num_queues;
            deq_tmp = deq(input->queues[chosen_queue]);
        }

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);

        }

    }

    while(true) {
        bool curr_cycle = false;
        for(int i = 0; i < input->num_queues; i++) {
            volatile Packet_t *deq_tmp = deq(input->queues[i]);
            if(deq_tmp) {
                curr_cycle = true;
                input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);
            }

        }
        if(curr_cycle == false) {
            break;
        }
    }
    pthread_exit(NULL);
}

void *awesome_worker(void *worker_args) {

    synchro *input = (synchro *) worker_args;
    srand(time(NULL));

    while(input->done == 0) {
        volatile Packet_t *deq_tmp = deq(input->queues[input->queue_index]);
        short r_tries = 0;
        while(deq_tmp == NULL && input->done == 0) {
            if(r_tries > 32) {
                deq_tmp = deq(input->queues[input->queue_index]);
                r_tries = 0;
            } else {
                int chosen_queue = (int) random() % input->num_queues;
                deq_tmp = fast_deq(input->queues[chosen_queue]);
                r_tries++;
            }


        }

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);

        }

    }
    volatile Packet_t *deq_tmp = deq(input->queues[input->queue_index]);
    while(deq_tmp != NULL) {
        input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);
        deq_tmp = deq(input->queues[input->queue_index]);

    }
    pthread_exit(NULL);
}

typedef void *(*worker_function)(void *);

worker_function getStrategy(int strategy) {
    switch (strategy) {
        default:
        case 0:
            return lock_free_worker;
        case 1:
            return home_queue_worker;
        case 2:
            return random_queue_worker;
        case 3:
            return awesome_worker;

    }
}



long parallelFirewall(int numPackets,
                      int numSources,
                      long mean,
                      int uniform,
                      short experimentNumber,
                      unsigned int queueDepth,
                      int lockType,
                      int strategy
) {
    PacketSource_t *packetSource = createPacketSource(mean, numSources, experimentNumber);
    StopWatch_t watch;
    long fingerprint = 0;
    int packets_processed = 0;

    startTimer(&watch);
    synchro threadData[numSources];
    volatile Packet_t *pending_packets[numSources];
    int packets_remaining[numSources];
    unsigned long tid[numSources];
    queue *queues[numSources];
    worker_function worker = getStrategy(strategy);
    lock_creator lock_new = getLockType(lockType);



    for(int l = 0; l < numSources; ++l) {
        pending_packets[l] = NULL;
        packets_remaining[l] = numPackets;
        queues[l] = createQueue(queueDepth, lock_new(numSources));

        threadData[l].fingerprint = 0;
        threadData[l].done = 0;
        threadData[l].num_queues = numSources;
        threadData[l].queue_index = l;
        threadData[l].queues = queues;

    }
    for(int i = 0; i < numSources; i++) {
        pthread_create(&tid[i], NULL, worker, &threadData[i]);
    }

    while(packets_processed < numPackets * numSources) {
        for(int j = 0; j < numSources; j++) {
            if(packets_remaining[j] > 0) {
                if(pending_packets[j] != NULL) {
                    int res = enq(pending_packets[j], queues[j]);
                    if(res != -1) {
                        pending_packets[j] = NULL;
                        packets_processed++;
                        packets_remaining[j]--;
                    }
                } else {
                    volatile Packet_t *tmp;
                    switch (uniform) {
                        default:
                        case 0:
                            tmp = getUniformPacket(packetSource, j);
                            break;
                        case 1:
                            tmp = getExponentialPacket(packetSource, j);
                            break;
                    }
                    int res = enq(tmp, queues[j]);
                    if(res == -1) {
                        pending_packets[j] = tmp;
                    } else {
                        packets_processed++;
                        packets_remaining[j]--;
                    }

                }
            }
        }
    }


    for(int i = 0; i < numSources; i++) {
        threadData[i].done = 1;
        pthread_join(tid[i], NULL);
    }

    for(int k = 0; k < numSources; ++k) {
        fingerprint += threadData[k].fingerprint;
    }
    stopTimer(&watch);

    deletePacketSource(packetSource);
    printf("%f\n%ld\n", getElapsedTime(&watch), fingerprint);
    return fingerprint;
}