#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 23 # number of cores
#SBATCH --ntasks-per-node=23
#SBATCH -t 11:00:00
#SBATCH --wait 0
#SBATCH --job-name=15c_d
#SBATCH --mem 65536

cd ~/project
make clean
make all
export OMP_NUM_THREADS=23
./depthfirst_parallel --dataset=datasets/gr17_d.txt --cities=17
# ./depthfirst_parallel --dataset=datasets/five_d.txt --cities=5
