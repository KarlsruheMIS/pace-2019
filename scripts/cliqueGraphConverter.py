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
m = 0

adjMatrix = [[0 for i in range(n)] for i in range(n)]

for line in f:
    if not (line.startswith("p") or line.startswith("c")):
        u = int(line.split()[0]) - 1
        v = int(line.split()[1]) - 1
        if u==v:
            continue
        newEdge = False
        if adjMatrix[u][v] != 1:
            adjMatrix[u][v] = 1
            newEdge = True
        if adjMatrix[v][u] != 1:
            adjMatrix[v][u] = 1
            newEdge = True
        if newEdge:
            m += 1

# print("[IN] n=" + str(n) + " m=" + str(m))

f.close()

f = open(os.path.join("cliqueGraphs", os.path.basename(graphFile)), 'w')

m = int((n*(n-1)/2) - m)

f.write("p edge " + str(n) + " " + str(m) + "\n")
for u in range(n):
    for v in range(n):
        if u==v:
            continue
        if adjMatrix[u][v] == 0:
            f.write("e " + str(u + 1) + " " + str(v + 1) + "\n")
            adjMatrix[v][u] = 1

# print("[OUT] n=" + str(n) + " m=" + str(m))

f.close()

