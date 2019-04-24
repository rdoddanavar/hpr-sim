#!/usr/bin/env bash
# update_index.sh

FILE="/home/${USER}/Git/hpr-sim/README.md"
BLK='```'
LVL=1 # No. lvl in tree

# Delete old index 
sed "1,/$BLK/!d" $FILE > temp
mv temp $FILE

# Generate new index, append to file
cd ..
tree -L $LVL --dirsfirst >> $FILE
echo "$BLK" >> $FILE
