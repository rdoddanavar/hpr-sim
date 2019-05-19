#!/usr/bin/env bash
# update_index.sh
# Takes one positional argument - tree depth level (int)
# ex. (2 levels) ./update_index.sh 2

target="/home/${USER}/Git/hpr-sim/"
file="/home/${USER}/Git/hpr-sim/README.md"
blk='```'
lvl=$1 # No. lvl in tree

# Delete old index 
sed "1,/$blk/!d" $file > temp
mv temp $file

# Generate new index, append to file
cd $target
tree -F -L $lvl --dirsfirst >> $file
echo "$blk" >> $file
