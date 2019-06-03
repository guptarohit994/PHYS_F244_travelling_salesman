#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH --job-name=depthfirst_hybrid_n9_c2_i1
#SBATCH --ntasks=9
#SBATCH --cpus-per-task=2
#SBATCH --nodes=9
#SBATCH --time=0-00:30# time (D-HH:MM) 
#SBATCH --output=depthfirst_hybrid_n9_c2_i1_%j.out
#SBATCH -e %j.err

export OMP_NUM_THREADS=2
export MV2_ENABLE_AFFINITY=0
make clean
make all
mpirun ./depthfirst_hybrid --dataset=./datasets/custom_11_d.txt --cities=11 --outfile=depthfirst_hybrid_n9_c2_i1
