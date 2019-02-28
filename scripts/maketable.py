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
    return round(result,2)

def getDataOurs(directory, filename):
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

def getDataAkiba(directory, filename):
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

def getDataDarren(directory, filename):
    filename,extension = os.path.splitext(filename)
    filename = filename + ".hgr" + extension

    time = getTime(directory, filename)

    f = open(os.path.join(directory, "output", filename), 'r')

    FinalSolution = -1

    for line in f:
        words = line.split()
        if line.startswith("vc-"):
            FinalSolution = int(words[7])

    if FinalSolution == -1:
        time = 7200.01

    f.close()

    return FinalSolution, time

def getDataClique(directory, filename):
    time = getTime(directory, filename)

    if not os.path.isfile(os.path.join(directory, "output", filename)):
        return -1, 7200.01
    f = open(os.path.join(directory, "output", filename), 'r')

    FinalSolution = -1

    for line in f:
        words = line.split()
        if line.startswith("s"):
            FinalSolution = int(words[4])

    if FinalSolution == -1:
        time = 7200.01

    f.close()

    return FinalSolution, time

def getDataWeighted(directory, filename):
    filename,extension = os.path.splitext(filename)
    filename = filename + ".hgr" + extension

    time = getTime(directory, filename)

    f = open(os.path.join(directory, "output", filename), 'r')

    FinalSolution = -1

    for line in f:
        words = line.split()
        if line.startswith("%timeout"):
            FinalSolution = -1
            break
        if line.startswith("%MIS_weight"):
            FinalSolution = int(words[1])
        if line.startswith("%nodes"):
            n = float(words[1])
        if line.startswith("%edges"):
            m = float(words[1])

    density = round(2*m/n, 2)

    if FinalSolution == -1:
        time = 7200.01

    f.close()

    return FinalSolution, time, density


table = []

for filename in os.listdir("Combine/output"):

    # print("COMBINE")
    n, LinearTimeKernel, FastKerKernel, VCSolverKernel, LocaSearchSolution, FinalSolution, validFlag, withoutMappingTime = getDataOurs("Combine", filename)
    # print("AKIBA")
    FinalSolution2, akibaTime = getDataAkiba("Akiba", filename)
    if not FinalSolution2 == -1 and not FinalSolution == -1 and FinalSolution2 != FinalSolution:
        print("ERROR in [Akiba] solution size of " + filename + ": " + str(FinalSolution) + " instead of " + str(FinalSolution2))
    # print("DARREN")
    FinalSolution3, darrenTime = getDataDarren("Darren", filename)
    if not FinalSolution3 == -1 and not FinalSolution == -1 and FinalSolution3 != FinalSolution:
        print("ERROR in [Darren] solution size of " + filename + ": " + str(FinalSolution) + " instead of " + str(FinalSolution3))
    FinalSolution4, cliqueTime = getDataClique("Clique_momc", filename)
    if not FinalSolution4 == -1 and not FinalSolution == -1 and FinalSolution4 != FinalSolution:
        print("ERROR in [Clique] solution size of " + filename + ": " + str(FinalSolution) + " instead of " + str(FinalSolution4))
    # FinalSolution5, darrenGLPTime = getDataDarren("Darren_glp", filename)
    # if not FinalSolution5 == -1 and not FinalSolution == -1 and FinalSolution5 != FinalSolution:
    #     print("ERROR in [Darren(GLP)] solution size of " + filename + ": " + str(FinalSolution) + " instead of " + str(FinalSolution5))
    # print("WEIGHTED(S)")
    FinalSolution6, weightedSparseTime,_ = getDataWeighted("Weighted_sparse", filename)
    if not FinalSolution6 == -1 and not FinalSolution == -1 and FinalSolution6 != FinalSolution:
        print("ERROR in [Sparse] solution size of " + filename + ": " + str(FinalSolution) + " instead of " + str(FinalSolution6))
    # print("WEIGHTED(D)")
    FinalSolution7, weightedDenseTime,density = getDataWeighted("Weighted_dense", filename)
    if not FinalSolution7 == -1 and not FinalSolution == -1 and FinalSolution7 != FinalSolution:
        print("ERROR in [Dense] solution size of " + filename + ": " + str(FinalSolution) + " instead of " + str(FinalSolution7))

    # FinalSolution = max(FinalSolution, FinalSolution2, FinalSolution3, FinalSolution4, FinalSolution5, FinalSolution6, FinalSolution7)
    FinalSolution = max(FinalSolution, FinalSolution2, FinalSolution3, FinalSolution4, FinalSolution6, FinalSolution7)

    # table.append([filename.replace(".output", ""), n, density, withoutMappingTime, akibaTime, darrenTime, cliqueTime, darrenGLPTime, weightedSparseTime, weightedDenseTime, VCSolverKernel, LocaSearchSolution, FinalSolution])
    table.append([filename.replace(".output", ""), n, density, withoutMappingTime, akibaTime, darrenTime, cliqueTime, weightedSparseTime, weightedDenseTime, VCSolverKernel, LocaSearchSolution, FinalSolution])

table = sorted(table,key=lambda l:l[2])

noMappingScore = 0
akibaScore = 0
darrenScore = 0
cliqueScore = 0
darrenGLPScore = 0
weightedSparseScore = 0
weightedDenseScore = 0

for line in table:
    if(line[3] < 1800):
        noMappingScore += line[3]
    else:
        noMappingScore += 10 * 1800

    if(line[4] < 1800):
        akibaScore += line[4]
    else:
        akibaScore += 10 * 1800

    if(line[5] < 1800):
        darrenScore += line[5]
    else:
        darrenScore += 10 * 1800

    if(line[6] < 1800):
        cliqueScore += line[6]
    else:
        cliqueScore += 10 * 1800

    if(line[7] < 1800):
        weightedSparseScore += line[7]
    else:
        weightedSparseScore += 10 * 1800

    if(line[8] < 1800):
        weightedDenseScore += line[8]
    else:
        weightedDenseScore += 10 * 1800

for line in table:
    fat = []
    line[0] = line[0].replace("_", "\\_")
    if(line[3] < 7200 and line[3] == min(line[7], line[3], line[4], line[5], line[6], line[8])):
        fat.append(3)
    if(line[4] < 7200 and line[4] == min(line[7], line[3], line[4], line[5], line[6], line[8])):
        fat.append(4)
    if(line[5] < 7200 and line[5] == min(line[7], line[3], line[4], line[5], line[6], line[8])):
        fat.append(5)
    if(line[6] < 7200 and line[6] == min(line[7], line[3], line[4], line[5], line[6], line[8])):
        fat.append(6)
    if(line[7] < 7200 and line[7] == min(line[7], line[3], line[4], line[5], line[6], line[8])):
        fat.append(7)
    if(line[8] < 7200 and line[8] == min(line[7], line[3], line[4], line[5], line[6], line[8])):
        fat.append(8)
    for l in fat:
        line[l] = "\\textbf{%s}" % line[l]

table.append(["\hline"])
# table.append(["Score", 0, 0, round(noMappingScore,2), round(akibaScore,2), round(darrenScore,2), round(weightedSparseScore,2), round(weightedDenseScore,2) ,-1, -1, -1])
# table.append(["Score", 0, 0, round(noMappingScore,2), round(akibaScore,2), round(darrenScore,2), round(cliqueScore,2), round(darrenGLPScore,2), round(weightedSparseScore,2), round(weightedDenseScore,2) ,-1, -1, -1])
table.append(["Score", 0, 0, round(noMappingScore,2), round(akibaScore,2), round(darrenScore,2), round(cliqueScore,2), round(weightedSparseScore,2), round(weightedDenseScore,2) ,-1, -1, -1])
f = open("table.tex", 'w')

tabulate.LATEX_ESCAPE_RULES={}
# f.write(tabulate(table, headers=["graph", "n", "dens", "$t_C$", "$t_A$", "$t_D$", "$t_w\\mbox{(sparse)}$", "$t_w\\mbox{(dense)}$", "VCS", "LS", "FS"], tablefmt="latex_raw"))
# f.write(tabulate(table, headers=["graph", "n", "dens", "$t_C$", "$t_A$", "$t_D$", "$t_D\\mbox{(ILS)}$", "$t_D\\mbox{(GLP)}$", "$t_W\\mbox{(sparse)}$", "$t_W\\mbox{(dense)}$", "VCS", "LS", "FS"], tablefmt="latex_raw"))
f.write(tabulate(table, headers=["graph", "n", "dens", "$t_S$", "$t_A$", "$t_D$", "$t_C$", "$t_W\\mbox{(sparse)}$", "$t_W\\mbox{(dense)}$", "VCS", "LS", "FS"], tablefmt="latex_raw"))

f.close()
