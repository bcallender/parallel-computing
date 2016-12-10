//
// Created by brandon on 1/25/16.
//

#include <check.h>
#include "queue.h"
#include "firewallImpl.h"

START_TEST(test_queue_seq)
    {
        queue *q = createQueue(5);
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
        queue *q = createQueue(1);
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
        queue *q = createQueue(32);
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

START_TEST(firewall_correctness)
    {
        printf("Serial Firewall\n");
        long c_fingerprint = serialFirewall(10, 3, 200, 0, 0);
        printf("Parallel Firewall 32 \n");
        long p1_fingerprint = parallelFirewall(10, 3, 200, 0, 0, 32);
        printf("Parallel Firewall 64 \n");
        long p2_fingerprint = parallelFirewall(10, 3, 200, 0, 0, 64);
        printf("Parallel Firewall 128 \n");
        long p3_fingerprint = parallelFirewall(10, 3, 200, 0, 0, 128);
        printf("Parallel Firewall 1\n");
        long p4_fingerprint = parallelFirewall(10, 3, 200, 0, 0, 1);

        printf("Serial Queue Firewall 32\n");
        long sq_fingerprint = serialQueueFirewall(10, 3, 200, 0, 0, 32);

        ck_assert_int_eq(c_fingerprint, p1_fingerprint);
        ck_assert_int_eq(p1_fingerprint, p2_fingerprint);
        ck_assert_int_eq(c_fingerprint, sq_fingerprint);
        ck_assert_int_eq(p2_fingerprint, p3_fingerprint);
        ck_assert_int_eq(p3_fingerprint, p4_fingerprint);
    }
END_TEST

Suite *queue_suite(void) {
    Suite *s;
    TCase *tc_core;
    s = suite_create("Queue");

    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_queue_seq);
    tcase_add_test(tc_core, test_queue_overfill);
    tcase_add_test(tc_core, firewall_correctness);
    tcase_add_test(tc_core, test_small_queue);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {

    Suite *s;
    SRunner *sr;
    int number_failed;

    s = queue_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
