#!/bin/bash

for program in vc_solver ; do 
scons program=$program variant=optimized -j 8 
if [ "$?" -ne "0" ]; then 
        echo "compile error in $program. exiting."
        exit
fi
done

