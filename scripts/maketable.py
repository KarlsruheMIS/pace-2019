#!python3

import os
from tabulate import tabulate

def getTime(directory, filename):
    result = -1
    f = open(os.path.join(directory, "parallel_log"), 'r')
    for line in f:
        if filename in line:
            result = float(line.split()[3])
            break
    f.close()
    return result

def getDataOurs(directory, filenname):
    time = getTime(directory, filename)

    f = open(os.path.join(directory, "output", filename), 'r')

    n = -1
    LinearTimeKernel = -1
    FastKerKernel = -1
    VCSolverKernel = -1
    LocaSearchSolution = -1
    FinalSolution = -1
    validFlag = False

    for line in f:
        words = line.split()
        if line.startswith("|-Nodes:"):
            n = int(words[1])
        if line.startswith("Degree_two_path MIS:"):
            LinearTimeKernel = int(words[5].split(',')[0][1:])
        if line.startswith("FastKer kernel size:"):
            FastKerKernel = int(words[3])
        if line.startswith("VCSolver kernel size:"):
            VCSolverKernel = int(words[3])
        if line.startswith("Local search found solution of size"):
            LocaSearchSolution = int(words[6])
        if line.startswith("Final solution size:"):
            FinalSolution = int(words[3])
        if line.startswith("Actual solution size:"):
            assert(int(words[3]) == FinalSolution)
        if line.startswith("Valid solution"):
            validFlag = True
        if line.startswith("ERROR! Invalid solution"):
            print("ERROR in " + filename)
            validFlag = False


    f.close()

    if time >= 7200:
        FinalSolution = -1

    return [n, LinearTimeKernel, FastKerKernel, VCSolverKernel, LocaSearchSolution, FinalSolution, validFlag, time]

def getDataAkiba(directory, filenname):
    time = getTime(directory, filename)

    f = open(os.path.join(directory, "output", filename), 'r')

    n = -1
    FinalSolution = -1

    for line in f:
        words = line.split()
        if line.startswith("opt = "):
            FinalSolution = int(words[2][:-1])
        if line.startswith("n = "):
            n = int(words[2][:-1])

    # if "123" in filenname:
    #     print(Fin)

    if not FinalSolution == -1:
        FinalSolution = n - FinalSolution

    if FinalSolution == -1:
        time = 7200.01

    f.close()

    return FinalSolution, time


table = []

for filename in os.listdir("with_mapping/output"):

    n, LinearTimeKernel, FastKerKernel, VCSolverKernel, LocaSearchSolution, FinalSolution, validFlag, withMappingTime = getDataOurs("with_mapping", filename)
    _, _, _, _, _, FinalSolution2, _, withoutMappingTime = getDataOurs("no_mapping", filename)
    if not FinalSolution2 == FinalSolution:
        print("Difference in solution size of " + filename + ": " + str(FinalSolution) + " and " + str(FinalSolution2))
    FinalSolution3, akibaTime = getDataAkiba("akiba", filename)
    if not FinalSolution3 == FinalSolution:
        print("ERROR in solution size of " + filename + ": " + str(FinalSolution) + " instead of " + str(FinalSolution3))

    FinalSolution = max(FinalSolution, FinalSolution2, FinalSolution3)


    table.append([filename.replace(".output", ""), n, withoutMappingTime, withMappingTime, akibaTime ,LinearTimeKernel, FastKerKernel, VCSolverKernel, LocaSearchSolution, FinalSolution])

table = sorted(table,key=lambda l:l[2])
noMappingScore = 0
withMappingScore = 0
akibaScore = 0

for line in table:
    if(line[2] < 1800):
        noMappingScore += line[2]
    else:
        noMappingScore += 10 * 1800

    if(line[3] < 1800):
        withMappingScore += line[3]
    else:
        withMappingScore += 10 * 1800

    if(line[4] < 1800):
        akibaScore += line[4]
    else:
        akibaScore += 10 * 1800

for line in table:
    line[0] = line[0].replace("_", "\\_")
    if(line[2] < 7200 and line[2] < line[3] and line[2] < line[4]):
        line[2] = "\\textbf{%s}" % line[2]
    elif(line[3] < 7200 and line[3] < line[2] and line[3] < line[4]):
        line[3] = "\\textbf{%s}" % line[3]
    elif(line[4] < 7200 and line[4] < line[2] and line[4] < line[3]):
        line[4] = "\\textbf{%s}" % line[4]

table.append(["Score", 0, noMappingScore, withMappingScore, akibaScore ,-1, -1, -1, -1, -1])
f = open("table.tex", 'w')

tabulate.LATEX_ESCAPE_RULES={}
f.write(tabulate(table, headers=["graph", "n", "t no mapping", "t with mapping", "t VCSolver", "LT", "FK", "VCS", "LS", "FS"], tablefmt="latex_raw"))

f.close()
