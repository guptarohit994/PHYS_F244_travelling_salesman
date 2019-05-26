#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 23 # number of cores
#SBATCH -t 0-10:00 # time (D-HH:MM)
#SBATCH --job-name=15c_d
#SBATCH -o %j.out # STDOUT
#SBATCH -e %j.err # STDERR
#SBATCH --mem 65536

# mail alert at start, end and abortion of execution
#SBATCH --mail-type=ALL

# send mail to this address
#SBATCH --mail-user=r4gupta@ucsd.edu

cd /oasis/projects/nsf/csd453/r4gupta/PHY_F244_travelling_salesman/
make clean
make
./run_multiple.sh depthfirst_parallel 15
