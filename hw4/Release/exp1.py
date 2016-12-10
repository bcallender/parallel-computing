#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array
import csv
from statistics import median

test_locks = [0, 1, 2, 3, 4]

def performExperiment(e, res_writer):
    N = 15
    results = []
    for r in range(0,11):
        print("Parallel No Load N:{0}".format(N))
        proc = generate_subprocess_array(v=2, W=1, n=15)
        s_time = check_output(proc).decode('utf-8')
        r = parse_results(s_time, e=r, load=2, numThreads=15, work=1, contains=0, tableType=2)
        results.append(r)
        print(s_time)
        res_writer.writerow(["NO LOAD", r.packets])


    res_writer.writerow(["Averages"])
    s_agg = median(sorted(map(lambda r: r.packets, results)))
    res_writer.writerow(['NO LOAD', str(N), s_agg])




def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        performExperiment(e, res_writer)

print("Experiment 1")
create_performance_csv("experiment1.csv", 1)
