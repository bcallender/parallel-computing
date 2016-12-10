#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array
import csv
from statistics import median

test_work = [1000, 2000, 4000, 8000]
test_sources = [1, 2, 4, 8, 16, 32, 64]


def performExperiment(e, res_writer):
    results = []
    for r in range(0, 5):
        for w in test_work:
            for n in test_sources:
                t = int(pow(2,17))
                print("Parallel N:{0} W:{1}".format(n, w))
                proc = generate_subprocess_array(n, t, w, d=32, e=e, v=0, p=1)
                p_time = check_output(proc).decode('utf-8')
                print(p_time)
                r = parse_results(p_time, e, 'Parallel', n, w)
                results.append(r)
                res_writer.writerow([r.type, str(n), str(w), r.fingerprint, r.time])
                print("Serial N:{0} W:{1}".format(n, w))
                proc = generate_subprocess_array(n, t, w, d=32, e=e, v=0, p=0)
                s_time = check_output(proc).decode('utf-8')
                r = parse_results(s_time, e, 'Serial', n, w)
                results.append(r)
                res_writer.writerow([r.type, str(n), str(w), r.fingerprint, r.time])
                print(s_time)
    res_writer.writerow(["Averages"])
    for n in test_sources:
            for w in test_work:
                p_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.type == 'Parallel' and r.n == n and r.w == w,
                                                          results))))
                s_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.type == 'Serial' and r.n == n and r.w == w,
                                                          results))))
                res_writer.writerow(['Parallel', str(n), str(w), 0, p_agg])
                res_writer.writerow(['Serial', str(n), str(w), 0, s_agg])




def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Type", "Sources", "Work", "Fingerprint", "Time"])
        performExperiment(e, res_writer)

print("Experiment 4")
create_performance_csv("experiment4.csv", 4)