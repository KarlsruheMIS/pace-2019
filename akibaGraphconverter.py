#!/usr/bin/python

import sys
import os

graphFile = ""

# print(sys.argv)

for arg in sys.argv:
    # print(arg)
    if arg.endswith(".hgr"):
        graphFile = arg
        break


f = open(graphFile, 'r')


n = -1

for line in f:
    if line.startswith("p td"):
        n = int(line.split()[2])
        break

edges = [[] for i in range(n)]

for line in f:
    if not (line.startswith("p") or line.startswith("c")):
        u = int(line.split()[0]) - 1
        v = int(line.split()[1]) - 1
        if not v in edges[u]:
            edges[u].append(v)
        if not u in edges[v]:
            edges[v].append(u)

for line in edges:
    line.sort()

f.close()

f = open(os.path.join("akibaGraphs", os.path.basename(graphFile)), 'w')


for u in range(n):
    for v in edges[u]:
        f.write(str(u) + " " + str(v) + "\n")

f.close()

