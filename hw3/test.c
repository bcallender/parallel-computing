//
// Created by brandon on 1/25/16.
//

#include <check.h>
#include <stdbool.h>
#include "queue.h"
#include "firewallImpl.h"
#include "counterImpl.h"

START_TEST(test_queue_seq)
    {
        lock *l = tas_init(1);
        queue *q = createQueue(5, l);
        volatile Packet_t *p3_null = deq(q);


        ck_assert_int_eq(NULL, p3_null);
        Packet_t *p1 = malloc(sizeof(Packet_t));
        p1->seed = 1;
        p1->iterations = 1;
        Packet_t *p2 = malloc(sizeof(Packet_t));
        p2->seed = 2;
        p2->iterations = 2;
        Packet_t *p3 = malloc(sizeof(Packet_t));
        p3->seed = 3;
        p3->iterations = 3;
        ck_assert_int_eq(enq(p1, q), 0);
        volatile Packet_t *p1_test = deq(q);
        ck_assert_int_eq(p1->seed, p1_test->seed);
        ck_assert_int_eq(p1->iterations, p1_test->iterations);
        ck_assert_int_eq(1, p1_test->seed);
        ck_assert_int_eq(1, p1_test->iterations);
        ck_assert_int_eq(enq(p2, q), 0);
        ck_assert_int_eq(enq(p3, q), 0);

        volatile Packet_t *p2_test = deq(q);
        volatile Packet_t *p3_test = deq(q);


        ck_assert_int_eq(p2->seed, p2_test->seed);
        ck_assert_int_eq(p3->seed, p3_test->seed);

    }
END_TEST

START_TEST(test_small_queue)
    {
        lock* l = tas_init(1);
        queue *q = createQueue(1, l);
        volatile Packet_t *p_null = deq(q);
        ck_assert_int_eq(NULL, p_null);
        Packet_t *p1 = malloc(sizeof(Packet_t));
        p1->seed = 1;
        p1->iterations = 1;
        Packet_t *p2 = malloc(sizeof(Packet_t));
        p2->seed = 2;
        p2->iterations = 2;
        Packet_t *p3 = malloc(sizeof(Packet_t));
        p3->seed = 3;
        p3->iterations = 3;
        ck_assert_int_eq(enq(p1, q), 0);
        volatile Packet_t *p1_test = deq(q);
        ck_assert_int_eq(p1->seed, p1_test->seed);
        ck_assert_int_eq(p1->iterations, p1_test->iterations);
        ck_assert_int_eq(1, p1_test->seed);
        ck_assert_int_eq(1, p1_test->iterations);
        ck_assert_int_eq(enq(p2, q), 0);
        ck_assert_int_eq(enq(p3, q), -1);

        volatile Packet_t *p2_test = deq(q);
        volatile Packet_t *p3_null = deq(q);


        ck_assert_int_eq(p2->seed, p2_test->seed);
        ck_assert_int_eq(NULL, p3_null);

    }
END_TEST

START_TEST(test_queue_overfill)
    {
        lock* l = tas_init(1);
        queue *q = createQueue(32, l);
        for(int i = 0; i < 32; i++) {
            Packet_t *p1 = malloc(sizeof(Packet_t));
            p1->seed = i;
            p1->iterations = i;
            ck_assert_int_eq(enq(p1, q), 0);
        }
        Packet_t *p33 = malloc(sizeof(Packet_t));

        p33->seed = 33;
        p33->iterations = 33;
        ck_assert_int_eq(enq(p33, q), -1);
        volatile Packet_t *p1_test = deq(q);
        ck_assert_int_eq(0, p1_test->seed);
        ck_assert_int_eq(0, p1_test->iterations);

        ck_assert_int_eq(enq(p33, q), 0);


    }
END_TEST

START_TEST(firewall_correctness_lock_free)
    {
        printf("LOCK FREE STRATEGY\n");
        printf("Serial Firewall\n");
        long c_fingerprint = serialFirewall(100, 3, 200, 0, 0);
        printf("Parallel Firewall 32 TAS Lock-Free\n");
        long p1_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 0, 0);
        printf("Parallel Firewall 32 Backoff Lock-Free\n");
        long p2_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 1, 0);
        printf("Parallel Firewall 32 ALock Lock-Free\n");
        long p3_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 2, 0);
        printf("Parallel Firewall 32 Mutex Lock-Free\n");
        long p4_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 3, 0);
        printf("Parallel Firewall 32 CLH Lock-Free\n");
        long p5_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 4, 0);
        printf("Parallel Firewall 1 Lock-Free\n");
        long p6_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 1, 0, 0);


        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
        ck_assert_int_eq(c_fingerprint, p2_fingerprint);
        ck_assert_int_eq(c_fingerprint, p3_fingerprint);
        ck_assert_int_eq(c_fingerprint, p4_fingerprint);
        ck_assert_int_eq(c_fingerprint, p5_fingerprint);
        ck_assert_int_eq(c_fingerprint, p6_fingerprint);

    }
END_TEST

START_TEST(firewall_correctness_home_queue)
    {
        printf("HOME QUEUE STRATEGY\n");
        printf("Serial Firewall\n");
        long c_fingerprint = serialFirewall(8192, 2, 25, 0, 0);
        printf("Parallel Firewall 32 TAS Home Queue\n");
        long p1_fingerprint = parallelFirewall(8192, 2, 25, 0, 0, 32, 0, 1);
        printf("Parallel Firewall 32 Backoff Home Queue\n");
        long p2_fingerprint = parallelFirewall(8192, 2, 25, 0, 0, 32, 1, 1);
        printf("Parallel Firewall 32 ALock Home Queue\n");
        long p3_fingerprint = parallelFirewall(8192, 2, 25, 0, 0, 32, 2, 1);
        printf("Parallel Firewall 32 Mutex Home Queue\n");
        long p4_fingerprint = parallelFirewall(8192, 2, 25, 0, 0, 32, 3, 1);
        printf("Parallel Firewall 32 CLH Home Queue\n");
        long p5_fingerprint = parallelFirewall(8192, 2, 25, 0, 0, 32, 4, 1);
        printf("Parallel Firewall 1 Home Queue\n");
        long p6_fingerprint = parallelFirewall(8192, 2, 25, 0, 0, 1, 0, 1);


        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
        ck_assert_int_eq(c_fingerprint, p2_fingerprint);
        ck_assert_int_eq(c_fingerprint, p3_fingerprint);
        ck_assert_int_eq(c_fingerprint, p4_fingerprint);
        ck_assert_int_eq(c_fingerprint, p5_fingerprint);
        ck_assert_int_eq(c_fingerprint, p6_fingerprint);

    }
END_TEST

START_TEST(firewall_correctness_home_queue_alock_sad)
    {
        printf("HOME QUEUE STRATEGY\n");
        printf("Serial Firewall\n");
        long c_fingerprint = serialFirewall(8192, 2, 25, 0, 0);
        printf("Parallel Firewall 32 ALock Home Queue\n");
        long p1_fingerprint = parallelFirewall(16384, 1, 25, 0, 0, 32, 2, 1);
        printf("Parallel Firewall 32 ALock Home Queue\n");
        long p2_fingerprint = parallelFirewall(8192, 2, 25, 0, 0, 32, 2, 1);


        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
        ck_assert_int_eq(c_fingerprint, p2_fingerprint);

    }
END_TEST

START_TEST(firewall_correctness_random_queue)
    {
        printf("RANDOM QUEUE STRATEGY\n");
        printf("Serial Firewall\n");
        long c_fingerprint = serialFirewall(100, 3, 200, 0, 0);
        printf("Parallel Firewall 32 TAS Random Queue\n");
        long p1_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 0, 2);
        printf("Parallel Firewall 32 Backoff Random Queue\n");
        long p2_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 1, 2);
        printf("Parallel Firewall 32 ALock Random Queue\n");
        long p3_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 2, 2);
        printf("Parallel Firewall 32 Mutex Random Queue\n");
        long p4_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 3, 2);
        printf("Parallel Firewall 32 CLH Random Queue\n");
        long p5_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 32, 4, 2);
        printf("Parallel Firewall 1 Random Queue\n");
        long p6_fingerprint = parallelFirewall(100, 3, 200, 0, 0, 1, 0, 2);


        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
        ck_assert_int_eq(c_fingerprint, p2_fingerprint);
        ck_assert_int_eq(c_fingerprint, p3_fingerprint);
        ck_assert_int_eq(c_fingerprint, p4_fingerprint);
        ck_assert_int_eq(c_fingerprint, p5_fingerprint);
        ck_assert_int_eq(c_fingerprint, p6_fingerprint);

    }
END_TEST

START_TEST(firewall_correctness_awesome)
    {
        printf("AWESOME QUEUE STRATEGY\n");
        printf("Serial Firewall Awesome\n");
        long c_fingerprint = serialFirewall(1000, 3, 200, 0, 0);
        printf("Parallel Firewall 32 TAS Awesome\n");
        long p1_fingerprint = parallelFirewall(1000, 3, 200, 0, 0, 32, 0, 3);
        printf("Parallel Firewall 32 Backoff Awesome\n");
        long p2_fingerprint = parallelFirewall(1000, 3, 200, 0, 0, 32, 1, 3);
        printf("Parallel Firewall 32 ALock Awesome\n");
        long p3_fingerprint = parallelFirewall(1000, 3, 200, 0, 0, 32, 2, 3);
        printf("Parallel Firewall 32 Mutex Awesome\n");
        long p4_fingerprint = parallelFirewall(1000, 3, 200, 0, 0, 32, 3, 3);
        printf("Parallel Firewall 32 CLH Awesome\n");
        long p5_fingerprint = parallelFirewall(1000, 3, 200, 0, 0, 32, 4, 3);
        printf("Parallel Firewall 1 Awesome\n");
        long p6_fingerprint = parallelFirewall(1000, 3, 200, 0, 0, 1, 0, 3);


        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
        ck_assert_int_eq(c_fingerprint, p2_fingerprint);
        ck_assert_int_eq(c_fingerprint, p3_fingerprint);
        ck_assert_int_eq(c_fingerprint, p4_fingerprint);
        ck_assert_int_eq(c_fingerprint, p5_fingerprint);
        ck_assert_int_eq(c_fingerprint, p6_fingerprint);

    }
END_TEST

START_TEST(tas_lock_test)
    {
        printf("TAS Lock Test\n");
        lock *lock = tas_init(0);
        ck_assert_int_eq(lock->tryLock(lock), true);
        ck_assert_int_eq(lock->tryLock(lock), false);
        lock->unlock(lock);
        ck_assert_int_eq(lock->tryLock(lock), true);
    }
END_TEST

START_TEST(backoff_lock_test)
    {
        printf("Backoff Lock Test\n");
        lock *lock = backoff_init(0);
        ck_assert_int_eq(lock->tryLock(lock), true);
        ck_assert_int_eq(lock->tryLock(lock), false);
        lock->unlock(lock);
        ck_assert_int_eq(lock->tryLock(lock), true);
    }
END_TEST

START_TEST(alock_lock_test)
    {
        printf("ALock Lock Test\n");
        lock *lock = alock_init(4);
        ck_assert_int_eq(lock->tryLock(lock), true);
        ck_assert_int_eq(lock->tryLock(lock), false);
        lock->unlock(lock);
        ck_assert_int_eq(lock->tryLock(lock), true);
    }
END_TEST

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

START_TEST(clh_lock_test)
    {
        printf("CLH Lock Test\n");
        lock *lock = clh_init(0);
        ck_assert_int_eq(lock->tryLock(lock), true);
        ck_assert_int_eq(lock->tryLock(lock), false);
        lock->unlock(lock);
        ck_assert_int_eq(lock->tryLock(lock), true);
    }
END_TEST

START_TEST(serial_counter_test)
    {
        printf("Serial Counter Test\n");
        int testval = 10000000;
        Counter *test = serialCounter(testval);
        ck_assert_int_eq(test->count, testval);

    }
END_TEST

START_TEST(parallel_tas_counter_test)
    {
        printf("Parallel TAS Counter Test\n");
        int testval = 10000000;
        Counter *test = parallelCounter(testval, 1, &tas_init);
        ck_assert_int_eq(test->count, testval);
        Counter *test2 = parallelCounter(testval, 2, &tas_init);
        ck_assert_int_eq(test2->count, testval);
        Counter *test4 = parallelCounter(testval, 4, &tas_init);
        ck_assert_int_eq(test4->count, testval);
        Counter *test8 = parallelCounter(testval, 8, &tas_init);
        ck_assert_int_eq(test8->count, testval);

    }
END_TEST

START_TEST(parallel_backoff_counter_test)
    {
        printf("Parallel Backoff Counter Test\n");
        int testval = 10000000;
        Counter *test = parallelCounter(testval, 1, &backoff_init);
        ck_assert_int_eq(test->count, testval);
        Counter *test2 = parallelCounter(testval, 2, &backoff_init);
        ck_assert_int_eq(test2->count, testval);
        Counter *test4 = parallelCounter(testval, 4, &backoff_init);
        ck_assert_int_eq(test4->count, testval);
        Counter *test8 = parallelCounter(testval, 8, &backoff_init);
        ck_assert_int_eq(test8->count, testval);

    }
END_TEST

START_TEST(parallel_alock_counter_test)
    {
        printf("Parallel Alock Counter Test\n");
        int testval = 10000000;
        Counter *test = parallelCounter(testval, 1, &alock_init);
        ck_assert_int_eq(test->count, testval);
        Counter *test2 = parallelCounter(testval, 2, &alock_init);
        ck_assert_int_eq(test2->count, testval);
        Counter *test4 = parallelCounter(testval, 4, &alock_init);
        ck_assert_int_eq(test4->count, testval);
        Counter *test8 = parallelCounter(testval, 8, &alock_init);
        ck_assert_int_eq(test8->count, testval);

    }
END_TEST

START_TEST(parallel_mutex_counter_test)
    {
        printf("Parallel pthread_mutex Counter Test\n");
        int testval = 1000000;
        Counter *test = parallelCounter(testval, 1, &mutex_init);
        ck_assert_int_eq(test->count, testval);
        Counter *test2 = parallelCounter(testval, 2, &mutex_init);
        ck_assert_int_eq(test2->count, testval);
        Counter *test4 = parallelCounter(testval, 4, &mutex_init);
        ck_assert_int_eq(test4->count, testval);
        Counter *test8 = parallelCounter(testval, 8, &mutex_init);
        ck_assert_int_eq(test8->count, testval);

    }
END_TEST

START_TEST(parallel_clh_counter_test)
    {
        printf("Parallel CLH Counter Test\n");
        int testval = 1000000;
        Counter *test = parallelCounter(testval, 1, &clh_init);
        ck_assert_int_eq(test->count, testval);
        Counter *test2 = parallelCounter(testval, 2, &clh_init);
        ck_assert_int_eq(test2->count, testval);
        Counter *test4 = parallelCounter(testval, 4, &clh_init);
        ck_assert_int_eq(test4->count, testval);
        Counter *test8 = parallelCounter(testval, 8, &clh_init);
        ck_assert_int_eq(test8->count, testval);

    }
END_TEST

START_TEST(idle_tas_test)
    {
        printf("Parallel TAS Counter Test\n");
        int testval = 100000000;
        serialCounter(testval);
        Counter *test = parallelCounter(testval, 1, &tas_init);
        ck_assert_int_eq(test->count, testval);
        Counter *test2 = parallelCounter(testval, 1, &backoff_init);
        ck_assert_int_eq(test2->count, testval);
        Counter *test4 = parallelCounter(testval, 1, &alock_init);
        ck_assert_int_eq(test4->count, testval);
        Counter *test8 = parallelCounter(testval, 1, &mutex_init);
        ck_assert_int_eq(test8->count, testval);
        Counter *test16 = parallelCounter(testval, 1, &clh_init);
        ck_assert_int_eq(test16->count, testval);

    }
END_TEST


Suite *queue_suite(void) {
    Suite *s;
    TCase *tc_core;
    s = suite_create("Queue");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_queue_seq);
    tcase_add_test(tc_core, test_queue_overfill);
    tcase_add_test(tc_core, firewall_correctness_lock_free);
    tcase_add_test(tc_core, firewall_correctness_home_queue);
    tcase_add_test(tc_core, firewall_correctness_random_queue);
    tcase_add_test(tc_core, firewall_correctness_awesome);
    tcase_add_test(tc_core, test_small_queue);
    tcase_add_test(tc_core, tas_lock_test);
    tcase_add_test(tc_core, backoff_lock_test);
    tcase_add_test(tc_core, alock_lock_test);
    tcase_add_test(tc_core, mutex_lock_test);
    tcase_add_test(tc_core, clh_lock_test);
    tcase_add_test(tc_core, serial_counter_test);
    tcase_add_test(tc_core, parallel_tas_counter_test);
    tcase_add_test(tc_core, parallel_backoff_counter_test);
    tcase_add_test(tc_core, parallel_alock_counter_test);
    tcase_add_test(tc_core, parallel_mutex_counter_test);
    tcase_add_test(tc_core, parallel_clh_counter_test);
    tcase_add_test(tc_core, idle_tas_test);
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
    srunner_set_fork_status(sr, CK_FORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
