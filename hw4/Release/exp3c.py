#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array, loadConfigToString, tableTypeToString
import csv
from statistics import median

test_contains = [0.5, 0.75, 0.9, 0.99]
test_threads = [1, 2, 4, 8, 16, 32]


def performExperiment(e, res_writer):
    W= 4000
    T=1
    results = []
    for r in range(0,4):
        for cont in test_contains:
            for N in test_threads:
                print("Parallel Mostly Reads N:{0} T:{1} C: {2}".format(N, tableTypeToString(T), cont))
                proc = generate_subprocess_array(v=1, W=W, n=N, H=cont, a=0.09, r=0.01, T=T)
                try:
                    s_time = check_output(proc, timeout=30).decode('utf-8')
                    res = parse_results(s_time, e=r, load=0, numThreads=N, work=W, contains=cont, tableType=T)
                    results.append(res)
                    print(s_time)
                    res_writer.writerow(["Parallel",r, N, tableTypeToString(T), W, loadConfigToString(0), cont, res.packets])
                except Exception:
                    print("Failed -- Timed out")

                print("Parallel Mostly Writes N:{0} T:{1} C: {2}".format(N, tableTypeToString(T), cont))
                proc = generate_subprocess_array(v=1, W=W, n=N, H=cont, a=0.45, r=0.05, T=T)
                try:
                    s_time = check_output(proc, timeout=30).decode('utf-8')
                    res = parse_results(s_time, e=r, load=1, numThreads=N, work=W, contains=cont, tableType=T)
                    results.append(res)
                    print(s_time)
                    res_writer.writerow(["Parallel",r ,  N, tableTypeToString(T), W, loadConfigToString(1), cont, res.packets])
                except Exception:
                    print("Failed -- Timed out")



    res_writer.writerow(["Averages"])

    for c in test_contains:
        for L in range(0,2):
            for N in test_threads:
                s_agg = median(sorted(map(lambda r: r.packets, filter(lambda res: res.load is L
                                                                                  and res.contains is c
                                                                                  and res.n is N,
                                                                      results))))
                res_writer.writerow([tableTypeToString(T), c, N, W, loadConfigToString(L), s_agg])





def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Experiment Type", "Experiment Number", "Threads", "Table Type", "Work", "Load Type", "Contains", "Packets"])
        performExperiment(e, res_writer)

print("Experiment 3")
create_performance_csv("experiment3c.csv", 3)
