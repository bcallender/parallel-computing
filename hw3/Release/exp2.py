#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array, generate_counter_subprocess_array, parse_counter_results, lockTypeToString
import csv
from statistics import median

test_locks = [1]
num_threads = [1, 2, 4, 8, 16, 32]

def performExperiment(e, res_writer):
    T = 100000000
    results = []
    for r in range(0,5):
        print("Serial Counter T:{0}".format(T))
        proc = generate_counter_subprocess_array(False, 1, 5, T)
        s_time = check_output(proc).decode('utf-8')
        r = parse_counter_results(s_time, False, 5, 0, r)
        results.append(r)
        print(s_time)
        res_writer.writerow([r.LockType, str(0), str(T), r.value, r.time])

        for lockType in test_locks:
            for N in num_threads:
                print("Parallel Counter N:{0} T:{1}".format(N, T))
                proc = generate_counter_subprocess_array(True, N, lockType, T)
                s_time = check_output(proc).decode('utf-8')
                r = parse_counter_results(s_time, True, lockType, N, r)
                results.append(r)
                print(s_time)
                res_writer.writerow([r.LockType, str(N), str(T), r.value, r.time])


    res_writer.writerow(["Averages"])
    s_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.parallel is False, results))))
    res_writer.writerow(['Serial', str(T), str(1), s_agg])
    for n in num_threads:

            p_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.n is n and r.parallel is True,
                                                          results))))
            res_writer.writerow(['Parallel', str(T), str(n), lockTypeToString(1), p_agg])



def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Type", "Threads", "Expected", "Value", "Time"])
        performExperiment(e, res_writer)

print("Experiment 2")
create_performance_csv("experiment2.csv", 2)
