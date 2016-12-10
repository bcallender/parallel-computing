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
                case 1:
                    tmp = getExponentialPacket(packetSource, i);
                    break;
                default:
                    tmp = getConstantPacket(packetSource, i);
                    break;
            }
            fingerprint += getFingerprint(tmp->iterations, tmp->seed);
        }
    }
    stopTimer(&watch);

    printf("%f\n%ld\n", getElapsedTime(&watch), fingerprint);
    return fingerprint;
}

long serialQueueFirewall(int numPackets,
                         int numSources,
                         long mean,
                         int version,
                         short experimentNumber,
                         unsigned int queueDepth) {
    PacketSource_t *packetSource = createPacketSource(mean, numSources, experimentNumber);
    StopWatch_t watch;
    long fingerprint = 0;

    startTimer(&watch);
    synchro threadData[numSources];
    volatile Packet_t *pending_packets[numSources];
    for(int l = 0; l < numSources; ++l) {
        pending_packets[l] = NULL;

    }
    for(int i = 0; i < numSources; i++) {
        threadData[i].fingerprint = 0;
        threadData[i].q = createQueue(queueDepth);
    }
    for(int i = 0; i < numPackets; i++) {
        for(int j = 0; j < numSources; j++) {
            volatile Packet_t *tmp;
            switch (version) {
                case 0:
                    tmp = getUniformPacket(packetSource, j);
                    break;
                case 1:
                    tmp = getExponentialPacket(packetSource, j);
                    break;
                default:
                    tmp = getConstantPacket(packetSource, j);
                    break;
            }
            enq(tmp, threadData[j].q);
            volatile Packet_t *deq_tmp = deq(threadData[j].q);
            while(deq_tmp == NULL) {
                deq_tmp = deq(threadData[j].q);
            }
            threadData[j].fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);
        }
    }
    for(int k = 0; k < numSources; ++k) {
        fingerprint += threadData[k].fingerprint;
    }
    stopTimer(&watch);


    printf("%f\n%ld\n", getElapsedTime(&watch), fingerprint);
    return fingerprint;
}

void *parallelFirewallWorker(void *varargs) {

    synchro *input = (synchro *) varargs;
    while(input->done == 0) {
        volatile Packet_t *deq_tmp = deq(input->q);
        while(deq_tmp == NULL && input->done == 0) {
            deq_tmp = deq(input->q);
        }

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);

        }

    }
    volatile Packet_t *deq_tmp = deq(input->q);
    while(deq_tmp != NULL) {

        if(deq_tmp) {
            input->fingerprint += getFingerprint(deq_tmp->iterations, deq_tmp->seed);
        }

        deq_tmp = deq(input->q);

    }
    pthread_exit(NULL);

}

long parallelFirewall(int numPackets,
                      int numSources,
                      long mean,
                      int version,
                      short experimentNumber,
                      unsigned int queueDepth) {
    PacketSource_t *packetSource = createPacketSource(mean, numSources, experimentNumber);
    StopWatch_t watch;
    long fingerprint = 0;
    int packets_processed = 0;

    startTimer(&watch);
    synchro threadData[numSources];
    volatile Packet_t *pending_packets[numSources];
    int packets_remaining[numSources];
    unsigned long tid[numSources];

    for(int l = 0; l < numSources; ++l) {
        pending_packets[l] = NULL;

    }
    for(int l = 0; l < numSources; ++l) {
        packets_remaining[l] = numPackets;

    }
    for(int i = 0; i < numSources; i++) {
        threadData[i].fingerprint = 0;
        threadData[i].done = 0;
        threadData[i].q = createQueue(queueDepth);
    }
    for(int i = 0; i < numSources; i++) {
        pthread_create(&tid[i], NULL, parallelFirewallWorker, &threadData[i]);
    }

    while(packets_processed < numPackets * numSources) {
        for(int j = 0; j < numSources; j++) {
            if(packets_remaining[j] > 0) {
                if(pending_packets[j] != NULL) {
                    int res = enq(pending_packets[j], threadData[j].q);
                    if(res != -1) {
                        pending_packets[j] = NULL;
                        packets_processed++;
                        packets_remaining[j]--;
                    }
                } else {
                    volatile Packet_t *tmp;
                    switch (version) {
                        case 0:
                            tmp = getUniformPacket(packetSource, j);
                            break;
                        case 1:
                            tmp = getExponentialPacket(packetSource, j);
                            break;
                        default:
                            tmp = getConstantPacket(packetSource, j);
                            break;
                    }
                    int res = enq(tmp, threadData[j].q);
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