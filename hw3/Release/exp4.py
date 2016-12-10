#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array, generate_counter_subprocess_array, parse_counter_results, lockTypeToString, strategyToString, expToString
import csv
from statistics import median

test_work = [1000, 2000, 4000, 8000]
test_sources = [1, 2, 3, 7, 15]
test_locks = [0, 1, 2, 3, 4]
test_queues = [1, 2]
test_types = ['Serial', 'Parallel']
test_distributions=[0]

def performExperiment(e, res_writer):
    results = []
    t = int(pow(2,14))
    for r in range(0, 3):
        for w in test_work:
            for n in test_sources:
                for uu in test_distributions:
                    n_packets = int(t/n)
                    print("Serial N:{0} W:{1}".format(n, w))
                    proc = generate_subprocess_array(False, n, n_packets, w, d=32, e=e, u=uu, L=0, S=0)

                    p_time = check_output(proc).decode('utf-8')
                    print(p_time)
                    r = parse_results(res=p_time, e=e, type='Serial', numThreads=n, work=w, strategy=5, lockType=5, exp_unf=uu)
                    results.append(r)
                    res_writer.writerow([r.type, str(n), str(w), r.fingerprint, r.time, strategyToString(r.strategy), lockTypeToString(r.lockType)])

                    print("Lock Free N:{0} W:{1}".format(n, w))
                    proc = generate_subprocess_array(True, n, n_packets, w, d=32, e=e, u=uu, L=5, S=0)

                    p_time = check_output(proc).decode('utf-8')
                    print(p_time)
                    r = parse_results(res=p_time, e=e, type='Serial', numThreads=n, work=w, strategy=5, lockType=5,  exp_unf=uu)
                    results.append(r)
                    res_writer.writerow([r.type, str(n), str(w), r.fingerprint, r.time, strategyToString(r.strategy), lockTypeToString(r.lockType)])
                    for l in test_locks:
                        for s in test_queues:
                            n_packets = int(t/n)
                            try:
                                print("{0} N:{1} W:{2}".format(strategyToString(s),n, w))
                                proc = generate_subprocess_array(True, n, n_packets, w, d=32, e=e, u=uu, L=l, S=s)
                                p_time = check_output(proc, timeout=5).decode('utf-8')
                                print(p_time)
                                r = parse_results(res=p_time, e=e, type='Parallel', numThreads=n, work=w, strategy=s, lockType=l,  exp_unf=uu)
                                results.append(r)
                                res_writer.writerow([r.type, str(n), str(w), r.fingerprint, r.time, strategyToString(r.strategy), lockTypeToString(r.lockType)])
                            except Exception:
                                pass

    res_writer.writerow(["Averages"])
    for n in test_sources:
        for w in test_work:
            for uu in test_distributions:
                try:
                    s_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.n == n and r.w == w and r.type is 'Serial'
                                                                       and r.exp_unf == uu,
                                                                      results))))

                    res_writer.writerow(['Serial', strategyToString(1), lockTypeToString(l), expToString(uu), str(n), str(w), str(t), 0, s_agg])

                    s_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.n == n and r.w == w and r.strategy is 0
                                                                       and r.type is 'Parallel' and r.exp_unf == uu,
                                                                      results))))

                    res_writer.writerow(['Parallel', strategyToString(0), lockTypeToString(5), expToString(uu), str(n), str(w), str(t), 0, s_agg])
                except Exception:
                    pass

                for s in test_queues:
                    for l in test_locks:
                        try:
                            p_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.n == n and r.w == w and r.strategy is s
                                                                               and r.lockType is l and r.type is 'Parallel'
                                                                               and r.exp_unf == uu,
                                                                      results))))

                            res_writer.writerow(['Parallel', strategyToString(s), lockTypeToString(l), expToString(uu), str(n), str(w), str(t), 0, p_agg])
                        except Exception:
                            pass





def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Type", "Sources", "Work", "Fingerprint", "Time", "Strategy", 'Lock Type'])
        performExperiment(e, res_writer)

print("Experiment 4")
create_performance_csv("experiment4-low.csv", 4)