#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
from common import Result, parse_results, generate_subprocess_array
import csv
from statistics import median

test_work = [25, 50, 100, 200, 400, 800]
test_sources = [1, 8, 16]





def performExperiment(e, res_writer):
    results = []
    for r in range(0,5):
        for n in test_sources:
            for w in test_work:
                t = int(pow(2,24) / (n*w))
                print("Serial N:{0} W:{1}".format(n, w))
                proc = generate_subprocess_array(n, t, w, d=32, e=e*r, v=0, p=0)
                s_time = check_output(proc).decode('utf-8')
                r = parse_results(s_time, e, 'Serial', n, w)
                results.append(r)
                print(s_time)
                res_writer.writerow([r.type, str(n), str(w), r.fingerprint, r.time])
                print("Serial-Queue N:{0} W:{1}".format(n, w))
                proc = generate_subprocess_array(n, t, w, d=32, e=e*r, v=0, p=2)
                sq_time = check_output(proc).decode('utf-8')
                r = parse_results(sq_time, e, "Serial-Queue", n, w)
                results.append(r)
                print(sq_time)
                res_writer.writerow([r.type, str(n), str(w), r.fingerprint, r.time])


    res_writer.writerow(["Averages"])
    for n in test_sources:
            for w in test_work:
                s_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.type == 'Serial' and r.n == n and r.w == w,
                                                          results))))
                sq_agg = median(sorted(map(lambda r: r.time, filter(lambda r: r.type == 'Serial-Queue' and r.n == n and r.w == w,
                                                          results))))

                res_writer.writerow(['Serial', str(n), str(w), 0, s_agg])
                res_writer.writerow(['Serial-Queue', str(n), str(w), 0, sq_agg])



def create_performance_csv(filename, e):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Type", "Sources", "Work", "Fingerprint", "Time"])
        performExperiment(e, res_writer)

print("Experiment 1")
create_performance_csv("experiment1.csv", 1)


