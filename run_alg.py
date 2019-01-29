#!/usr/bin/python
import sys, os, re, math, subprocess, numpy, itertools

graph = sys.argv[1]
seed = sys.argv[2]
total_time = float(sys.argv[3])


script_path = os.path.dirname(os.path.realpath(__file__))
time_remaining = str(math.ceil(total_time))
print "%s/deploy/redumis %s --seed %s -time_limit %s" % (script_path, graph, seed, time_remaining)
output = subprocess.check_output([script_path + "/deploy/redumis", graph, "--seed", seed, "--time_limit", time_remaining, "--console_log"], stderr=subprocess.STDOUT)

for line in output.splitlines():
    args = list(filter(lambda a: not a in ["<", "0", ">"], line.split()))
    if len(args) == 2 and args[1].startswith("[") and args[1].endswith("]"):
        if args[0] == "[0.00]":
            continue
        mis_size = int(args[0])
        time_taken = float(args[1][1:-1])
        print "%d [%f]" % (mis_size, time_taken)
