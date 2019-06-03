#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 23 # number of cores
#SBATCH -t 0-10:00 # time (D-HH:MM)
#SBATCH --job-name=15c_d
#SBATCH -o %j.out # STDOUT
#SBATCH -e %j.err # STDERR
#SBATCH --mem 65536

cd /oasis/projects/nsf/csd453/roobee
make clean
make
./run_multiple.sh depthfirst_parallel 15
