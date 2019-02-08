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

m = 0

for line in f:
    if not (line.startswith("p") or line.startswith("c")):
        u = int(line.split()[0]) - 1
        v = int(line.split()[1]) - 1
        if u == v:
            continue
        if not v in edges[u]:
            edges[u].append(v)
            m += 1
        if not u in edges[v]:
            edges[v].append(u)
            m += 1

m /= 2

for line in edges:
    line.sort()

f.close()

f = open(os.path.join("metisGraphs", os.path.basename(graphFile) + ".graph"), 'w')

f.write("%s %s 1\n" % (n, m))

for u in range(n):
    line = ""
    for v in edges[u]:
        line += " " + str(v + 1)
    line = line.strip(' ')
    line += "\n"
    f.write(line)

f.close()
