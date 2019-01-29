#!/bin/bash
ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q /home/c_schulz/projects/coding/parallel_social_partitioning/src
cp tags /home/c_schulz/projects/coding/parallel_social_partitioning/src/app/tags
cp tags /home/c_schulz/projects/coding/parallel_social_partitioning/src/tags

DIRS=`find ./lib -type d | grep -v svn`
for dir in $DIRS; do 
cp tags $dir/tags
done

DIRS=`find ./app -type d | grep -v svn`
for dir in $DIRS; do 
cp tags $dir/tags
done

#cp tags /home/schulz/.vim/tags/kaffka
