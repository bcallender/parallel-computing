#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array, generate_counter_subprocess_array, parse_counter_results, lockTypeToString
import csv
from statistics import median

test_locks = [0, 1, 2, 3, 4]

def performExperiment(e, res_writer):
    T = 100000000
    N = 1
    results = []
    for r in range(0,5):
        print("Serial Counter N:{0} T:{1}".format(N, T))
        proc = generate_counter_subprocess_array(False, N, 5, T)
        s_time = check_output(proc).decode('utf-8')
        r = parse_counter_results(s_time, False, 5, N, r)
        results.append(r)
        print(s_time)
        res_writer.writerow([r.LockType, str(1), str(T), r.value, r.time])

        for lockType in test_locks:
            print("Parallel Counter N:{0} T:{1}".format(N, T))
            proc = generate_counter_subprocess_array(True, N, lockType, T)
            s_time = check_output(proc).decode('utf-8')
            r = parse_counter_results(s_time, True, lockType, N, r)
            results.append(r)
            print(s_time)
            res_writer.writerow([r.LockType, str(N), str(T), r.value, r.time])


    res_writer.writerow(["Averages"])
    s_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.parallel is False, results))))
    res_writer.writerow(['Serial', str(T), str(N), s_agg])
    for n in test_locks:

            p_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.LockType is n and r.parallel is True,
                                                          results))))
            res_writer.writerow(['Parallel', str(T), str(N), lockTypeToString(n), p_agg])



def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Type", "Threads", "Expected", "Value", "Time"])
        performExperiment(e, res_writer)

print("Experiment 1")
create_performance_csv("experiment1.csv", 1)
