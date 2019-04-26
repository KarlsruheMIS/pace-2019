#!/bin/bash

app="_kernel"

./optimized/kernelizeAndMakeCliqueInstance $1
./extern/cliqueSolver/a.out $1$app
rm $1$app
