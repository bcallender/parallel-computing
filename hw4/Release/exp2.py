#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array, loadConfigToString, tableTypeToString
import csv
from statistics import median

test_table_types = [0, 1]


def performExperiment(e, res_writer):
    N = 1
    W=4000
    results = []
    for r in range(0,5):
        print("Serial Mostly Reads N:{0}".format(N))
        proc = generate_subprocess_array(v=0, W=W, n=N, H=0.9, a=0.09, r=0.01)
        s_time = check_output(proc).decode('utf-8')
        res = parse_results(s_time, e=r, load=0, numThreads=N, work=W, contains=0.9, tableType=2)
        results.append(res)
        print(s_time)
        res_writer.writerow(["Serial",r , N, W, loadConfigToString(0), res.packets])

        print("Serial Mostly Writes N:{0}".format(N))
        proc = generate_subprocess_array(v=0, W=W, n=N, H=0.9, a=0.45, r=0.05)
        s_time = check_output(proc).decode('utf-8')
        res = parse_results(s_time, e=r, load=1, numThreads=N, work=W, contains=0.9, tableType=2)
        results.append(res)
        print(s_time)
        res_writer.writerow(["Serial",r , N, W, loadConfigToString(1), res.packets])

        for T in test_table_types:
            print("Parallel Mostly Reads N:{0} T:{1}".format(N, tableTypeToString(T)))
            proc = generate_subprocess_array(v=1, W=W, n=N, H=0.9, a=0.09, r=0.01, T=T)
            s_time = check_output(proc).decode('utf-8')
            res = parse_results(s_time, e=r, load=0, numThreads=N, work=W, contains=0.9, tableType=T)
            results.append(res)
            print(s_time)
            res_writer.writerow(["Parallel",r , N, W, loadConfigToString(0), res.packets])

            print("Parallel Mostly Writes N:{0} T:{1}".format(N, tableTypeToString(T)))
            proc = generate_subprocess_array(v=1, W=W, n=N, H=0.9, a=0.45, r=0.05, T=T)
            s_time = check_output(proc).decode('utf-8')
            res = parse_results(s_time, e=r, load=1, numThreads=N, work=W, contains=0.9, tableType=T)
            results.append(res)
            print(s_time)
            res_writer.writerow(["Parallel",r , N, W, loadConfigToString(1), res.packets])


    res_writer.writerow(["Averages"])
    for T in range(0,3):
        for L in range(0,2):
            s_agg = median(sorted(map(lambda r: r.packets, filter(lambda res: res.tableType is T and res.load is L, results))))
            res_writer.writerow([tableTypeToString(T),r , N, W, loadConfigToString(L), s_agg])




def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Experiment Type", "Experiment Number", "Threads", "Work", "Load Type", "Packets"])
        performExperiment(e, res_writer)

print("Experiment 2")
create_performance_csv("experiment2.csv", 2)
