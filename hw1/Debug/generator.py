#!/usr/bin/python3

from random import random, randint, choice
from subprocess import call, check_output
import time
from math import sqrt, floor
import csv

test_vals = [16, 32, 64, 128, 256, 512, 1024]
test_threads = [2, 4, 8, 16, 32, 64]


def create_nodes(num_nodes, sparseness, filename, span):
    graph = dict()
    span = list(span)
    for i in range(0, num_nodes):
        for j in range(0, num_nodes):
            if i == j:
                graph[(i, j)] = 0
            elif random() < sparseness:
                graph[(i, j)] = 10000000
            else:
                graph[(i, j)] = choice(span)
    print_graph(graph, filename)
    return graph


def import_graph(filename):
    graph = dict()
    f = open(filename, "r")
    num_nodes = int(f.readline().rstrip())
    line = f.readline()
    row = 0
    while line is not "":
        line = line.rstrip()
        tokens = line.split(" ")
        for col, token in enumerate(tokens):
            graph[(row, col)] = int(token.rstrip())
        row += 1
        line = f.readline()
    return graph


def floyd_warshall(graph, filename):
    results = dict(graph)
    num_nodes = floor(sqrt(len(list(graph.keys()))))
    for k in range(0, num_nodes):
        for i in range(0, num_nodes):
            for j in range(0, num_nodes):
                test_result = results[(i, k)] + results[(k, j)]
                current_shortest = results[(i, j)]
                if results[(i, j)] > test_result:
                    results[(i, j)] = test_result

    for i in range(0, num_nodes):
        if results[(i, i)] < 0:
            raise Exception("Negative-cost cycle found in graph. Exiting.")
    print_graph(results, filename)
    return results


def print_graph(graph, filename):
    f = open(filename, 'w+')
    num_nodes = floor(sqrt(len(list(graph.keys()))))
    print(str(num_nodes), file=f)
    for i in range(0, num_nodes):
        for j in range(0, num_nodes):
            print(graph[(i, j)], end=' ', file=f)
        print(file=f)


def generate_test_files(sizes):
    for size in sizes:
        print("starting generation of file {0}".format(size))
        t0 = time.time()
        g = create_nodes(size, 0.8, "graphInput" + str(size), range(0, 25))
        floyd_warshall(g, "graphOutputReference" + str(size))
        t1 = time.time()
        print("finished generation of file {0}. It took {1}".format(size, t1 - t0))


def simple_test():
    print("Testing Simple")
    print("Single Threaded")

    val = 32
    g = create_nodes(val, 0.8, "graphInput" + str(val), range(0, 25))
    og = floyd_warshall(g, "graphOutputReference" + str(val))
    serial_time = float(
            check_output(["./ParallelHW1", "-i", "graphInput" + str(val), "-o", "graphOutputTestSerial" + str(val)]))
    parallel_time = float(
            check_output(["./ParallelHW1", "-i", "graphInput" + str(val), "-o", "graphOutputTestParallel" + str(val)
                             , '-p', '-t', '32']))
    tg_serial = import_graph("graphOutputTestSerial" + str(val))
    tg_parallel = import_graph("graphOutputTestParallel" + str(val))
    if tg_serial == og:
        print("Serial {0} Passed in {1}!".format(val, str(serial_time)))
    else:
        print("Serial {0} Failed in {1}!".format(val, str(serial_time)))

    if tg_parallel == og:
        print("Parallel {0} Passed in {1}!".format(val, str(parallel_time)))
    else:
        print("Parallel {0} Failed in {1}!".format(val, str(parallel_time)))


def serial_test(val):
    print("Testing Serial")
    g = create_nodes(val, 0.8, "graphInput" + str(val), range(0, 25))
    og = floyd_warshall(g, "graphOutputReference" + str(val))
    serial_time = float(
            check_output(
                    ["./ParallelHW1", "-s", "-i", "graphInput" + str(val), "-o", "graphOutputTestSerial" + str(val)]))
    tg_serial = import_graph("graphOutputTestSerial" + str(val))

    if tg_serial == og:
        print("Serial {0} Passed in {1}!".format(val, str(serial_time)))
    else:
        print("Serial {0} Failed in {1}!".format(val, str(serial_time)))


def single_threaded_performance(writer):
    print("Testing Performance")
    print("Single Threaded")

    for val in test_vals:
        serial_time = check_output(
                generate_subprocess_array(val)).decode('utf-8')
        parallel_time = check_output(generate_subprocess_array(val, t=1, p=True)).decode('utf-8')
        tg_serial = import_graph("graphOutputTestSerial" + str(val))
        tg_parallel = import_graph("graphOutputTestParallel" + str(1) + str(val))
        og = import_graph("graphOutputReference" + str(val))
        if tg_serial == og:
            writer.writerow(["Serial", "Success", str(val), str(1), str(serial_time)])
            print("Serial {0} Passed in {1}!".format(val, str(serial_time)))
        else:
            writer.writerow(["Serial", "Failed", str(val), str(1), str(serial_time)])
            print("Serial {0} Failed in {1}!".format(val, str(serial_time)))

        if tg_parallel == og:
            writer.writerow(["Parallel", "Success", str(val), str(1), str(parallel_time)])
            print("Parallel {0} Passed in {1}!".format(val, str(parallel_time)))
        else:
            writer.writerow(["Parallel", "Failure", str(val), str(1), str(parallel_time)])
            print("Parallel {0} Failed in {1}!".format(val, str(parallel_time)))


def generate_subprocess_array(val, t=1, p=False):
    if p:
        return ["./ParallelHW1", "-p", "-i", "graphInput" + str(val), "-o",
                "graphOutputTestParallel" + str(t) + str(val)
            , '-t', str(t)]
    else:
        return ["./ParallelHW1", "-i", "graphInput" + str(val), "-o", "graphOutputTestSerial" + str(val)]


def multi_threaded_performance(writer):
    print("Testing Performance")
    print("Multi-Threaded")

    for val in test_vals:
        for t in test_threads:
            parallel_time = check_output(generate_subprocess_array(val, t, True)).decode('utf-8')
            tg_parallel = import_graph("graphOutputTestParallel" + str(t) + str(val))
            og = import_graph("graphOutputReference" + str(val))

            if tg_parallel == og:
                writer.writerow(["Parallel", "Success", str(val), str(t), str(parallel_time)])
                print("Parallel {0} with {1} threads Passed in {2}!".format(val, t, str(parallel_time)))

            else:
                writer.writerow(["Parallel", "Failure", str(val), str(t), str(parallel_time)])
                print("Parallel {0} with  {1} threads Failed in {2}".format(val, t, str(parallel_time)))


def create_performance_csv(filename):
    with open(filename, 'w') as csvfile:
        res_writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        res_writer.writerow(["Type", "Status", "Vertices", "Threads", "Time"])
        single_threaded_performance(res_writer)
        multi_threaded_performance(res_writer)

create_performance_csv("performanceTest.csv")
