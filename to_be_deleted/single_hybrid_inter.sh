export OMP_NUM_THREADS=2
export MV2_ENABLE_AFFINITY=0
make clean
make all
mpirun ./depthfirst_hybrid --dataset=./datasets/custom_11_d.txt --cities=11 --outfile=depthfirst_hybrid_n9_c2_i1
