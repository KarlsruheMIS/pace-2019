#!/bin/bash

rm -rf deploy 2>/dev/null

cd modKaHIP
./cleanup.sh
./compile.sh
cd ..

cp modKaHIP/deploy/libkahip.a parallel_social_partitioning/src/extern/kaHIP_lib/
cp modKaHIP/deploy/kaHIP_interface.h parallel_social_partitioning/src/extern/kaHIP_lib/

cd parallel_social_partitioning/src/
scons program=parallel_label_compress_reps variant=optimized -j 4 -c
scons program=parallel_label_compress_reps variant=optimized -j 4 
cd ..
cd ..

mkdir deploy
cp parallel_social_partitioning/src/optimized/parallel_label_compress_reps deploy/
