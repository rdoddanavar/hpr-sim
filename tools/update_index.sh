#!/usr/bin/env bash
# High Power Rocketry - Flight Simulation
# GNU General Public License v3.0
# Copyright (C) 2019 Roshan Doddanavar
# https://rdoddanavar.github.io
# 
# ██╗  ██╗██████╗ ██████╗       ███████╗██╗███╗   ███╗
# ██║  ██║██╔══██╗██╔══██╗      ██╔════╝██║████╗ ████║
# ███████║██████╔╝██████╔╝█████╗███████╗██║██╔████╔██║
# ██╔══██║██╔═══╝ ██╔══██╗╚════╝╚════██║██║██║╚██╔╝██║
# ██║  ██║██║     ██║  ██║      ███████║██║██║ ╚═╝ ██║
# ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝      ╚══════╝╚═╝╚═╝     ╚═╝
# 
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
#     hpr-sim/README.md
#------------------------------------------------------------#

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
