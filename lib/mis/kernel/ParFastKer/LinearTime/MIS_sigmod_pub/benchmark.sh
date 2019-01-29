#!/bin/bash
for filePath in ../../parallel_reductions/graphs/*.graph; do
    filename=$(basename "$filePath")
    echo $filename
    resultsfile=results/LinearTime/$filename
    for i in {1..1}; do
        echo $i
        ./mis LinearTime $filePath >> $resultsfile
        # graphname="${filename%.*}"
        # mv kernel.graph results/LinearTimeKernels/"$graphname"-LinearTimeKernel.graph
    done
done
