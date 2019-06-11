#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 1 # number of cores
#SBATCH -t 0-10:00 # time (D-HH:MM)
#SBATCH --job-name=hybrid_l_17
#SBATCH -o %j.out # STDOUT
#SBATCH -e %j.err # STDERR

module load python/2.7.10
cd /oasis/projects/nsf/csd453/roobee/
make clean
make all
python3 regress_hybrid.py --executable depthfirst_hybrid --cities=11
