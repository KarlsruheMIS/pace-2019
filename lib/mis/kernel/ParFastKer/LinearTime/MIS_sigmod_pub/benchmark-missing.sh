#!/bin/bash
for filePath in ../../parallel_reductions/graphs/*.graph; do
    filename=$(basename "$filePath")
    echo $filename
    resultsfile=results/NearLinear/$filename
    if [ -f $resultsfile ]; then
        continue
    fi
    for i in {1..1}; do
        echo $i
        ./mis NearLinear $filePath >> $resultsfile
        # graphname="${filename%.*}"
        # mv kernel.graph results/LinearTimeKernels/"$graphname"-LinearTimeKernel.graph
    done
done
