#!/usr/bin/python
import sys, os, re, getopt, random, argparse, math
import subprocess 
# import statistics 
import itertools
import matplotlib as mpl
import csv
import numpy as np

mpl.use('pgf')
latexConf = {
    "pgf.texsystem": "pdflatex",
    "font.family": "serif", # use serif/main font for text elements
    "font.serif": ["Computer Modern"], # use serif/main font for text elements
    # "axes.titlesize": "xx-large",   # fontsize of the axes title
    # "axes.labelsize": "xx-large",  # fontsize of the x any y labels
    # "xtick.labelsize": "xx-large",  # fontsize of the tick labels
    # "ytick.labelsize": "xx-large",  # fontsize of the tick labels
    # "legend.fontsize": "xx-large", #legend font size
    "axes.titlesize": "x-small",   # fontsize of the axes title
    "axes.labelsize": "x-small",  # fontsize of the x any y labels
    "xtick.labelsize": "x-small",  # fontsize of the tick labels
    "ytick.labelsize": "x-small",  # fontsize of the tick labels
    "legend.fontsize": "x-small", #legend font size
    "text.usetex": True,    # use inline math for ticks
    "pgf.rcfonts": False,   # don't setup fonts from rc parameters
    "pgf.preamble": [
        r"\renewcommand{\sfdefault}{phv}",
        r"\renewcommand{\rmdefault}{ptm}",
        r"\renewcommand{\ttdefault}{pcr}",
        r"\normalfont\selectfont"
    ]}
mpl.rcParams.update(latexConf)

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

def list_files(dir):
    r = []
    for root, dirs, files in os.walk(dir):
        files = [f for f in files if not f[0] == '.']
        for name in files:
            r.append(os.path.join(root, name))
    return r

def log_format(y,pos):
    decimalplaces = int(np.maximum(-np.log10(y),0))     # =0 for numbers >=1
    formatstring = '{{:.{:1d}f}}'.format(decimalplaces)
    return formatstring.format(y)

def update_colors(ax):
    lines = ax.lines
    colors = cm(np.linspace(0, 1, len(lines)))
    for line, c in zip(lines, colors):
        line.set_color(c)

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

#######################
# Plot data
#######################
def plot_density(data, ax):
    print("Generate plot...")

    ax.grid(True)
    # ax.set_title("Solving time over density")

    ax.set_xscale("log", basex=10)
    # ax.set_xlabel("Density $\\frac{2m}{n(n-1)}$")
    # ax.set_xlabel("Edges $m$")
    ax.set_xlabel("Metric")
    # ax.set_xlim(xmax=0.2)
    ax.get_xaxis().set_major_formatter(mpl.ticker.FuncFormatter(log_format))

    ax.set_yscale('log', basey=10)
    ax.set_ylabel("Time $t$ (s)")
    # ax.set_ylim(ymax=500)

    density = [2*float(x['m'])/(float(x['n'])*float(x['n']) - 1) for x in data]
    density_kernel = [2*float(x['m\''])/(float(x['n\''])*float(x['n\'']) - 1) for x in data]
    vertices = [float(x['n']) for x in data]
    vertices_kernel = [float(x['n\'']) for x in data]
    edges = [float(x['m']) for x in data]
    edges_kernel = [float(x['m\'']) for x in data]
    size = [float(x['n']) + float(x['m']) for x in data]
    size_kernel = [float(x['n\'']) + float(x['m\'']) for x in data]
    quotient = [float(x['n\'']) / float(x['n']) for x in data]

    for solver in solvers:
        times = sorted([float(x[solver]) for x in data])
        # valid_times = [x for x in times if x < 1800.0]
        ax.scatter(x=quotient, y=times, color=next(color), lw=1, s=10, marker=next(marker), label=solver_labels[solver])
    ax.legend(bbox_to_anchor=anchor, ncol=3, loc='center', borderaxespad=0.)
    # update_colors(ax)

def plot_solved(data, ax):
    print("Generate plot...")

    ax.grid(True)
    # ax.set_title("Instances solved over time")

    ax.set_xscale("log", basex=10)
    ax.set_xlabel("Time $t$ (s)")
    ax.get_xaxis().set_major_formatter(mpl.ticker.FuncFormatter(log_format))

    # ax.set_yscale("log", basey=2)
    ax.set_ylabel("Instances solved")
    ax.set_ylim(ymax=100)
    ax.yaxis.set_major_locator(plt.MaxNLocator(5))

    for solver in solvers:
        times = sorted([float(x[solver]) for x in data])
        valid_times = [x for x in times if x < 1800.0]
        instances_solved = range(1, len(valid_times) + 1)
        print(solver + ": " + str(len(valid_times) + 0))
        ax.plot(valid_times, instances_solved, lw=2, color=next(color), label=solver_labels[solver])
    ax.legend(bbox_to_anchor=solved_anchor, ncol=3, loc='center', borderaxespad=0.)
    # update_colors(ax)

def parse_file(input_file):
    with open(input_file, 'rb') as f:
        raw_data = csv.DictReader(f)
        data = list(raw_data)
    return sorted(data)

#######################
# Main
#######################

parser = argparse.ArgumentParser(description='Evaluate scaling of component algorithms.')
parser.add_argument('-input', type=str, nargs='?')
parser.add_argument('-output', type=str, nargs='?')
parser.add_argument('-s', type=str, nargs='?')
parser.add_argument('-g', type=str, nargs='?')
parser.add_argument('-l', type=str, nargs='+')
parser.add_argument('-f', type=str, nargs='?')
args = parser.parse_args()

input_file = args.input
output_file = args.output

cm = plt.get_cmap('gist_earth')
color = itertools.cycle(("g", "r", "b", "y", "c")) 
marker = itertools.cycle(("o", "v", "x", "d", "^")) 

solved_anchor = [0.5, -0.45]
anchor = [0.5, -0.6]

solvers = ['MoMC', 'RMoMC', 'FullBnR', 'BnrwithoutLS', 'FullAlgorithm']
solver_labels = {'MoMC': 'MoMC', 'RMoMC': 'RMoMC', 'FullBnR': 'LSBnR', 'BnrwithoutLS': 'BnR', 'FullAlgorithm': 'FullA'}

print("##########################")
print(bcolors.HEADER + "Generate PACE plots" + bcolors.ENDC)
print("##########################")


plt.rc('figure', figsize=(3, 1.75))

f, axarr = plt.subplots(1, 1)
plot_density(parse_file(input_file), axarr)
plt.savefig(output_file + "_density.pgf", bbox_inches="tight")

f, axarr = plt.subplots(1, 1)
plot_solved(parse_file(input_file), axarr)
plt.savefig(output_file + "_solutions.pgf", bbox_inches="tight")
