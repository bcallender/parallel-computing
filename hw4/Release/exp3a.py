#!/usr/bin/python3
from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array, loadConfigToString, tableTypeToString
import csv
from statistics import median

test_contains = [0.5, 0.75, 0.9, 0.99]

def performExperiment(e, res_writer):
    W= 4000
    results = []
    for r in range(0,4):
        for cont in test_contains:
            print("Serial Mostly Reads N:1 C:{0}".format(cont))
            proc = generate_subprocess_array(v=0, W=W, n=1, H=cont, a=0.09, r=0.01)
            s_time = check_output(proc).decode('utf-8')
            res = parse_results(s_time, e=r, load=0, numThreads=1, work=W, contains=cont, tableType=2)
            results.append(res)
            print(s_time)
            res_writer.writerow(["Serial",r, tableTypeToString(2), 1, W, loadConfigToString(0), cont, res.packets])

            print("Serial Mostly Writes N:1 C:{0}".format(cont))
            proc = generate_subprocess_array(v=0, W=W, n=1, H=cont, a=0.45, r=0.05)
            s_time = check_output(proc).decode('utf-8')
            res = parse_results(s_time, e=r, load=1, numThreads=1, work=W, contains=cont, tableType=2)
            results.append(res)
            print(s_time)
            res_writer.writerow(["Serial",r, tableTypeToString(2), 1, W, loadConfigToString(1), cont, res.packets])



    res_writer.writerow(["Averages"])

    for c in test_contains:
        for L in range(0,2):
                s_agg = median(sorted(map(lambda r: r.packets, filter(lambda res: res.contains is c
                                                                      and res.load is L,
                                                                      results))))
                res_writer.writerow([tableTypeToString(2), 1, c, W, loadConfigToString(L), s_agg])





def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Experiment Type", "Experiment Number", "Threads", "Table Type", "Work", "Load Type", "Contains", "Packets"])
        performExperiment(e, res_writer)

print("Experiment 3")
create_performance_csv("experiment3a.csv", 3)
