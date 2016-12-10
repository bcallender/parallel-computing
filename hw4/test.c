//
// Created by brandon on 1/25/16.
//

#include <check.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#include "queue.h"
#include "../lib/concurrentList.h"
#include "../lib/concurrenthashtable.h"
#include "../lib/coarsehashtable.h"
#include "firewallImpl.h"

START_TEST(test_queue_seq)
    {
        queue *q = createQueue(5);
        volatile HashPacket_t *p3_null = deq(q);


        ck_assert_int_eq(NULL, p3_null);
        HashPacket_t *hp1 = malloc(sizeof(HashPacket_t));

        Packet_t *p1 = malloc(sizeof(Packet_t));
        p1->seed = 1;
        p1->iterations = 1;
        hp1->body = p1;
        HashPacket_t *hp2 = malloc(sizeof(HashPacket_t));
        Packet_t *p2 = malloc(sizeof(Packet_t));
        p2->seed = 2;
        p2->iterations = 2;
        hp2->body = p2;
        HashPacket_t *hp3 = malloc(sizeof(HashPacket_t));
        Packet_t *p3 = malloc(sizeof(Packet_t));
        p3->seed = 3;
        p3->iterations = 3;
        hp3->body = p3;
        ck_assert_int_eq(enq(hp1, q), 0);
        volatile HashPacket_t *p1_test = deq(q);
        ck_assert_int_eq(1, p1_test->body->seed);
        ck_assert_int_eq(1, p1_test->body->iterations);
        ck_assert_int_eq(enq(hp2, q), 0);
        ck_assert_int_eq(enq(hp3, q), 0);

        volatile HashPacket_t *p2_test = deq(q);
        volatile HashPacket_t *p3_test = deq(q);


        ck_assert_int_eq(p2->seed, p2_test->body->seed);
        ck_assert_int_eq(p3->seed, p3_test->body->seed);

    }
END_TEST
//
//START_TEST(test_small_queue)
//    {
//        lock* l = mutex_init(1);
//        queue *q = createQueue(1, l);
//        volatile Packet_t *p_null = deq(q);
//        ck_assert_int_eq(NULL, p_null);
//        Packet_t *p1 = malloc(sizeof(Packet_t));
//        p1->seed = 1;
//        p1->iterations = 1;
//        Packet_t *p2 = malloc(sizeof(Packet_t));
//        p2->seed = 2;
//        p2->iterations = 2;
//        Packet_t *p3 = malloc(sizeof(Packet_t));
//        p3->seed = 3;
//        p3->iterations = 3;
//        ck_assert_int_eq(enq(p1, q), 0);
//        volatile Packet_t *p1_test = deq(q);
//        ck_assert_int_eq(p1->seed, p1_test->seed);
//        ck_assert_int_eq(p1->iterations, p1_test->iterations);
//        ck_assert_int_eq(1, p1_test->seed);
//        ck_assert_int_eq(1, p1_test->iterations);
//        ck_assert_int_eq(enq(p2, q), 0);
//        ck_assert_int_eq(enq(p3, q), -1);
//
//        volatile Packet_t *p2_test = deq(q);
//        volatile Packet_t *p3_null = deq(q);
//
//
//        ck_assert_int_eq(p2->seed, p2_test->seed);
//        ck_assert_int_eq(NULL, p3_null);
//
//    }
//END_TEST

START_TEST(test_queue_overfill)
    {
        queue *q = createQueue(32);
        for(int i = 0; i < 32; i++) {
            HashPacket_t *p1 = malloc(sizeof(HashPacket_t));
            p1->body = malloc(sizeof(Packet_t));
            p1->body->seed = i;
            p1->body->iterations = i;
            ck_assert_int_eq(enq(p1, q), 0);
        }
        HashPacket_t *p33 = malloc(sizeof(HashPacket_t));
        p33->body = malloc(sizeof(Packet_t));

        p33->body->seed = 33;
        p33->body->iterations = 33;
        ck_assert_int_eq(enq(p33, q), -1);
        volatile HashPacket_t *p1_test = deq(q);
        ck_assert_int_eq(0, p1_test->body->seed);
        ck_assert_int_eq(0, p1_test->body->iterations);

        ck_assert_int_eq(enq(p33, q), 0);


    }
END_TEST

//START_TEST(firewall_correctness_lock_free)
//    {
//        printf("LOCK FREE STRATEGY\n");
//        printf("Serial Firewall\n");
//        long c_fingerprint = serialFirewall(100, 3, 200, 0, 0);
//        printf("Parallel Firewall 32 TAS Lock-Free\n");
//        long p1_fingerprint = queueFirewall(100, 3, 200, 0, 0, 32, 0, 0);
//        printf("Parallel Firewall 32 Backoff Lock-Free\n");
//        long p2_fingerprint = queueFirewall(100, 3, 200, 0, 0, 32, 1, 0);
//        printf("Parallel Firewall 32 ALock Lock-Free\n");
//        long p3_fingerprint = queueFirewall(100, 3, 200, 0, 0, 32, 2, 0);
//        printf("Parallel Firewall 32 Mutex Lock-Free\n");
//        long p4_fingerprint = queueFirewall(100, 3, 200, 0, 0, 32, 3, 0);
//        printf("Parallel Firewall 32 CLH Lock-Free\n");
//        long p5_fingerprint = queueFirewall(100, 3, 200, 0, 0, 32, 4, 0);
//        printf("Parallel Firewall 1 Lock-Free\n");
//        long p6_fingerprint = queueFirewall(100, 3, 200, 0, 0, 1, 0, 0);
//
//
//        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p2_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p3_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p4_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p5_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p6_fingerprint);
//
//    }
//END_TEST
//
//START_TEST(firewall_correctness_home_queue)
//    {
//        printf("HOME QUEUE STRATEGY\n");
//        printf("Serial Firewall\n");
//        long c_fingerprint = serialFirewall(8192, 2, 25, 0, 0);
//        printf("Parallel Firewall 32 TAS Home Queue\n");
//        long p1_fingerprint = queueFirewall(8192, 2, 25, 0, 0, 32, 0, 1);
//        printf("Parallel Firewall 32 Backoff Home Queue\n");
//        long p2_fingerprint = queueFirewall(8192, 2, 25, 0, 0, 32, 1, 1);
//        printf("Parallel Firewall 32 ALock Home Queue\n");
//        long p3_fingerprint = queueFirewall(8192, 2, 25, 0, 0, 32, 2, 1);
//        printf("Parallel Firewall 32 Mutex Home Queue\n");
//        long p4_fingerprint = queueFirewall(8192, 2, 25, 0, 0, 32, 3, 1);
//        printf("Parallel Firewall 32 CLH Home Queue\n");
//        long p5_fingerprint = queueFirewall(8192, 2, 25, 0, 0, 32, 4, 1);
//        printf("Parallel Firewall 1 Home Queue\n");
//        long p6_fingerprint = queueFirewall(8192, 2, 25, 0, 0, 1, 0, 1);
//
//
//        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p2_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p3_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p4_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p5_fingerprint);
//        ck_assert_int_eq(c_fingerprint, p6_fingerprint);
//
//    }
//END_TEST



START_TEST(mutex_lock_test)
    {
        printf("mutex Lock Test\n");
        lock *lock = mutex_init(0);
        ck_assert_int_eq(lock->tryLock(lock), true);
        ck_assert_int_eq(lock->tryLock(lock), false);
        lock->unlock(lock);
        ck_assert_int_eq(lock->tryLock(lock), true);
    }
END_TEST

START_TEST(concurrent_list_simple)
    {
        printf("Concurrent List Simple Test\n");
        ConcurrentList_t *clist = createConcurrentList();
        Packet_t p1;
        p1.seed = 1;
        p1.iterations = 1;
        ck_assert_int_eq(addConcurrent(clist, 1, &p1), true);
        ck_assert_int_eq(clist->size, 1);
        ck_assert_int_eq(addConcurrent(clist, 1, &p1), false);
        ck_assert_int_eq(clist->size, 1);
        ck_assert_int_eq(containsConcurrent(clist, 1), true);
        ck_assert_int_eq(removeConcurrent(clist, 1), true);
        ck_assert_int_eq(removeConcurrent(clist, 1), false);
        ck_assert_int_eq(clist->size, 0);

        for(int i = 0; i < 32; i++) {
            Packet_t pp;
            pp.seed = i;
            pp.iterations = i;
            int key = firstLevelHash(i);
            ck_assert_int_eq(addConcurrent(clist, key, &pp), true);
            ck_assert_int_eq(clist->size, i + 1);
        }

        for(int i = 31; i >= 0; i--) {
            int key = firstLevelHash(i);
            ck_assert_int_eq(containsConcurrent(clist, key), true);
            ck_assert_int_eq(removeConcurrent(clist, key), true);
            ck_assert_int_eq(clist->size, i);
        }


    }
END_TEST

START_TEST(serial_firewall)
    {
        printf("Serial Firewall\n");
        serialFirewall(5000, 0, 2000,
                       16, 16, 0.09, 0.01, 0.9);
    }
END_TEST

START_TEST(parallel_coarse_firewall)
    {
        printf("Parallel Coarse Firewall\n");
        queueFirewall(5000, 0, 2000,
                      16, 16, 0.09, 0.01, 0.9, 0, false);
    }
END_TEST

START_TEST(parallel_concurrent_firewall)
    {
        printf("Parallel Concurrent Firewall\n");
        queueFirewall(5000, 0, 2000,
                      16, 16, 0.09, 0.01, 0.9, 1, false);
    }
END_TEST


START_TEST(concurrent_hash_table_simple)
    {
        printf("Concurrent HashTable Simple Test\n");
        ConcurrentHashTable_t *cht = createConcurrentHashTable(4, 0.75, 4, 8);
        int i = 0;
        for (i = 1; i < 65536; i++) {
            Packet_t pp;
            pp.seed = i;
            pp.iterations = i;
            int key = firstLevelHash(i);
            (add_cht(cht, key, &pp), true);
        }

        for (int j = 1; j < 65536; j++) {

            ck_assert_int_eq(contains_cht(cht, firstLevelHash(j)), true);
            ck_assert_int_eq(remove_cht(cht, firstLevelHash(j)), true);
            ck_assert_int_eq(contains_cht(cht, firstLevelHash(j)), false);

        }

    }
END_TEST

START_TEST(coarse_hash_table_simple)
    {
        printf("Coarse HashTable Simple Test\n");
        CoarseHashTable_t *cht = createCoarseHashTable(4, 0.75, 8, mutex_init);

        for(int i = 0; i < 80000; i++) {
            Packet_t pp;
            pp.seed = i;
            pp.iterations = i;
            int key = firstLevelHash(i);
            (add_coarse(cht, key, &pp), true);
        }

        for(int i = 0; i < 80000; i++) {

            ck_assert_int_eq(contains_coarse(cht, firstLevelHash(i)), true);
            ck_assert_int_eq(remove_coarse(cht, firstLevelHash(i)), true);
            ck_assert_int_eq(contains_coarse(cht, firstLevelHash(i)), false);

        }

    }
END_TEST

START_TEST(serial_list_simple)
    {
        printf("Serial List Simple Test\n");
        SerialList_t *slist = createSerialList();

        for(int i = 0; i < 5000; i++) {
            Packet_t *pp = malloc(sizeof(Packet_t));
            pp->seed = i;
            pp->iterations = i;
            int key = firstLevelHash(i);
            ck_assert_int_eq(add_list(slist, key, &pp), true);
            ck_assert_int_eq(add_list(slist, key, &pp), false);
            ck_assert_int_eq(slist->size, i + 1);

        }

        for(int i = 0; i < 5000; i++) {

            ck_assert_int_eq(contains_list(slist, firstLevelHash(i)), true);
            ck_assert_int_eq(remove_list(slist, firstLevelHash(i)), true);
            ck_assert_int_eq(remove_list(slist, firstLevelHash(i)), false);
            ck_assert_int_eq(contains_list(slist, firstLevelHash(i)), false);
            ck_assert_int_eq(slist->size, 5000 - i - 1);

        }

    }
END_TEST

START_TEST(serial_list_random_remove)
    {
        printf("Serial List Simple Test\n");
        SerialList_t *slist = createSerialList();

        for(int i = 0; i < 10; i++) {
            Packet_t *pp = malloc(sizeof(Packet_t));
            pp->seed = i;
            pp->iterations = i;
            int key = firstLevelHash(i);
            ck_assert_int_eq(add_list(slist, key, &pp), true);
            ck_assert_int_eq(add_list(slist, key, &pp), false);
            ck_assert_int_eq(slist->size, i + 1);

        }
        int randOrder[10] = {0, 4, 1, 2, 3, 6, 9, 8, 5, 7};

        for(int i = 0; i < 10; i++) {
            ck_assert_int_eq(contains_list(slist, firstLevelHash(randOrder[i])), true);
            ck_assert_int_eq(remove_list(slist, firstLevelHash(randOrder[i])), true);
            ck_assert_int_eq(remove_list(slist, firstLevelHash(randOrder[i])), false);
            ck_assert_int_eq(contains_list(slist, firstLevelHash(randOrder[i])), false);
            ck_assert_int_eq(slist->size, 10 - i - 1);
        }


    }
END_TEST

typedef struct {
    int start;
    int finish;
    int t_num;
    ConcurrentHashTable_t *cht;
} hash_worker_data;

void *hash_adder_only(void *worker_args) {
    hash_worker_data *data = (hash_worker_data *) worker_args;
    //sleep(data->t_num);
    for(int i = data->start; i < data->finish; i++) {
        Packet_t *pp = malloc(sizeof(Packet_t));
        pp->seed = i;
        pp->iterations = i;
        add_cht(data->cht, i, pp);
    }
    pthread_exit(NULL);
}


void *hash_adder_worker(void *worker_args) {
    hash_worker_data *data = (hash_worker_data *) worker_args;
    //sleep(data->t_num);
    for(int i = data->start; i < data->finish; i++) {
        Packet_t *pp = malloc(sizeof(Packet_t));
        pp->seed = i;
        pp->iterations = i;
        (add_cht(data->cht, i, pp), true);
    }
    for(int i = data->start; i < data->finish; i++) {
        int pindex = partitionForKey(data->cht, i);
        int hashKey = firstLevelHash(i);
        int ppIndex = firstLevelHash(i) & data->cht->partitions[pindex]->mask;
        assert(contains_cht(data->cht, (i)) == true);
        assert(remove_cht(data->cht, (i)) == true);
        assert(contains_cht(data->cht, (i)) == false);

    }

    pthread_exit(NULL);

}

START_TEST(concurrent_hash_table_multi)
    {
        printf("Concurrent HashTable Multi-Threaded Test\n");
        ConcurrentHashTable_t *cht = createConcurrentHashTable(3, 0.75, 3, 32);
        int numThreads = 8;
        pthread_t tid[numThreads];
        for(int i = 0; i < numThreads; i++) {

            hash_worker_data *data = malloc(sizeof(hash_worker_data));
            data->start = 10000 * i;
            data->finish = 10000 * (i + 1);
            data->cht = cht;
            data->t_num = numThreads - i;
            pthread_create(&tid[i], NULL, (void *) &hash_adder_worker, data);
        }

        for(int i = 0; i < numThreads; i++) {
            pthread_join(tid[i], NULL);
        }
        for(int i = 0; i < (1 << cht->partitionLogSize); i++) {
            ck_assert_int_eq(cht->partitions[i]->numEntries, 0);

        }

    }
END_TEST

START_TEST(concurrent_hash_table_adds)
    {
        printf("Concurrent HashTable Multi-Threaded Adds\n");
        ConcurrentHashTable_t *cht = createConcurrentHashTable(3, 0.75, 4, 32);
        int numThreads = 8;
        pthread_t tid[numThreads];
        int workPerThread = 20000;
        for(int j = 0; j < 10; j++) {
            for(int i = 0; i < numThreads; i++) {

                hash_worker_data *data = malloc(sizeof(hash_worker_data));
                data->start = workPerThread * i;
                data->finish = workPerThread * (i + 1);
                data->cht = cht;
                data->t_num = numThreads - i;
                pthread_create(&tid[i], NULL, (void *) &hash_adder_only, data);
            }

            for(int i = 0; i < numThreads; i++) {
                pthread_join(tid[i], NULL);
            }
            for (int i = 0; i < workPerThread * (numThreads); i++) {
                ck_assert_int_eq(contains_cht(cht, i), true);
                ck_assert_int_eq(remove_cht(cht, (i)), true);
                ck_assert_int_eq(contains_cht(cht, i), false);
            }
            for (int i = 0; i < (1 << cht->partitionLogSize); i++) {
                ck_assert_int_eq(cht->partitions[i]->numEntries, 0);

            }

        }
    }
END_TEST

typedef struct {
    int start;
    int finish;
    ConcurrentList_t *clist;
    int t_num;
} clist_worker_data;


void *clist_worker(void *worker_args) {
    clist_worker_data *data = (clist_worker_data *) worker_args;
    //sleep(data->t_num);
    for(int i = data->start; i < data->finish; i++) {
        Packet_t pp;
        pp.seed = i;
        pp.iterations = i;
        assert(addConcurrent(data->clist, i, &pp) == true);

    }
    for(int i = data->start; i < data->finish; i++) {

        assert(containsConcurrent(data->clist, (i)) == true);
        assert(removeConcurrent(data->clist, (i)) == true);
        assert(containsConcurrent(data->clist, (i)) == false);

    }
    pthread_exit(NULL);

}


START_TEST(concurrent_list_multithreaded)
    {
        printf("Concurrent List Multi-Threaded Test\n");
        ConcurrentList_t *clist = createConcurrentList();
        int numThreads = 8;
        pthread_t tid[numThreads];
        for(int i = 0; i < numThreads; i++) {

            clist_worker_data *data = malloc(sizeof(clist_worker_data));
            data->start = 1000 * i;
            data->finish = 1000 * (i + 1);
            data->clist = clist;
            data->t_num = numThreads - i;


            pthread_create(&tid[i], NULL, (void *) &clist_worker, data);
        }
        for(int i = 0; i < numThreads; i++) {
            pthread_join(tid[i], NULL);
        }
    }
END_TEST


Suite *queue_suite(void) {
    Suite *s;
    TCase *tc_core;
    s = suite_create("Queue");
    tc_core = tcase_create("Core");
//    tcase_add_test(tc_core, test_queue_seq);
//    tcase_add_test(tc_core, test_queue_overfill);
//    tcase_add_test(tc_core, mutex_lock_test);
//    tcase_add_test(tc_core, coarse_hash_table_simple);
//    tcase_add_test(tc_core, serial_list_simple);
//    tcase_add_test(tc_core, serial_list_random_remove);
//    tcase_add_test(tc_core, concurrent_list_simple);
    tcase_add_test(tc_core, concurrent_hash_table_simple);
    tcase_add_test(tc_core, concurrent_hash_table_multi);
    tcase_add_test(tc_core, concurrent_hash_table_multi);
    tcase_add_test(tc_core, concurrent_list_multithreaded);
    tcase_add_test(tc_core, concurrent_hash_table_adds);
//    tcase_add_test(tc_core, serial_firewall);
//    tcase_add_test(tc_core, parallel_coarse_firewall);
//    tcase_add_test(tc_core, parallel_concurrent_firewall);

    tcase_set_timeout(tc_core, 30.0);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {


    Suite *s;
    SRunner *sr;
    int number_failed;

    s = queue_suite();
    sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
