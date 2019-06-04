# PHY_F244_travelling_salesman

`make `
This builds all the variants - depthfirst_serial, depthfirst_parallel, depthfirst_serial_recursion_dfs

`make depthfirst_serial`
This builds the serial brute force algorithm that implements iterative DFS

`make depthfirst_parallel`
This builds the parallel brute force algorithm that implements iterative DFS

`make depthfirst_serial_recursion_dfs`
This builds a seria brute force algorithm that implements recursive DFS

`make clean`
This removes all the binaries built.

`./run_multiple.sh <executable> <city count>`
This runs the provided executable with different number of threads 10 times, returning the average wall time taken

### Recipe for running hybrid program manually

There are two flags that should be set - `export OMP_NUM_THREADS=<num>` and `export MV2_ENABLE_AFFINITY=0`

After that request nodes using the following sample script - 

```bash
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
mpirun ./depthfirst_hybrid --dataset=./datasets/custom_11_d.txt --cities=11 --outfile=depthfirst_hybrid_n9_c2_i1
```

>The above script requests 9 nodes and each node can have 2 cores that can be utilized by OpenMP


### Recipe for running hybrid program using regression script

> Uses python3, so do `module load python/2.7.10`

The regression script is named - `regress_hybrid.py`. Following is the help - 
```bash
usage: regression.py [-h] --executable EXECUTABLE [EXECUTABLE ...]
                     [--cities CITIES] [--iteration [ITERATION]]

Optional app description

optional arguments:
  -h, --help            show this help message and exit
  --executable EXECUTABLE [EXECUTABLE ...]
                        Executable to use
  --cities CITIES       Dataset to use:5
  --iteration [ITERATION]
                        Iterations per configuration, default:5
```

Sample command line for the script (**although the script can't be launched on login node**) - 
```bash
module load python/2.7.10
python3 regression.py --executable depthfirst_hybrid --cities=5
```

For launching the script, use the following batch script - 
```bash
#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 1 # number of cores
#SBATCH -t 0-10:00 # time (D-HH:MM)
#SBATCH --job-name=hybrid_l_17
#SBATCH -o %j.out # STDOUT
#SBATCH -e %j.err # STDERR

# mail alert at start, end and abortion of execution
#SBATCH --mail-type=ALL

# send mail to this address
#SBATCH --mail-user=r4gupta@ucsd.edu

module load python/2.7.10

cd /oasis/projects/nsf/csd453/r4gupta/test/PHY_F244_travelling_salesman/
python3 regression.py --executable depthfirst_hybrid --cities=11
```
This script will internally launch sbatch jobs with the identified configurations and wait until all jobs are finished to display the summary.

More datasets - https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html