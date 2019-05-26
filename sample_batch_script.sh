#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 23 # number of cores
#SBATCH -t 0-15:00 # time (D-HH:MM)
#SBATCH --job-name=15c_d
#SBATCH -o eleven.out # STDOUT
#SBATCH -e eleven.err # STDERR
#SBATCH --mem 65536

cd ~/project
make clean
make all
./run_multiple.sh depthfirst_parallel 11
