#!/usr/bin/env bash
#------------------------------------------------------------#
# High Power Rocketry - Flight Simulation
# GNU General Public License v3.0
# Roshan Doddanavar
# https://rdoddanavar.github.io
#------------------------------------------------------------#
#     ___       ___       ___       ___       ___       ___
#    /\__\     /\  \     /\  \     /\  \     /\  \     /\__\  
#   /:/__/_   /::\  \   /::\  \   /::\  \   _\:\  \   /::L_L_
#  /::\/\__\ /::\:\__\ /::\:\__\ /\:\:\__\ /\/::\__\ /:/L:\__\
#  \/\::/  / \/\::/  / \;:::/  / \:\:\/__/ \::/\/__/ \/_/:/  /
#    /:/  /     \/__/   |:\/__/   \::/  /   \:\__\     /:/  /
#    \/__/               \|__|     \/__/     \/__/     \/__/  
# 
#------------------------------------------------------------#
# Path:
#     hpr-sim/tools/update_index.sh
# Created:
#     2019-04-23
# Type:
#     Bash script
# Description:
#     Uses tree program to generate directory index, to update project README 
#     See also: https://linux.die.net/man/1/tree   
# Input:
#     tree depth, as an integer
#     Ex. `./update_index 2`
# Output:
#     None
# Dependencies:
#     hpr-sim/
#         README.md
#------------------------------------------------------------#

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
