# PHYS_F244_travelling_salesman

### Steps to build the binaries

`make `
This builds all the variants - depthfirst_serial, depthfirst_parallel, depthfirst_serial_recursion_dfs, depthfirst_hybrid

`make depthfirst_serial`
This builds the serial brute force algorithm that implements iterative DFS

`make depthfirst_parallel`
This builds the parallel brute force algorithm that implements iterative DFS

`make depthfirst_serial_recursion_dfs`
This builds a seria brute force algorithm that implements recursive DFS

`make depthfirst_hybrid`

`make clean`
This removes all the binaries built.


### Sample command line for Serial (DFS Iterative) Algorithm

`./depthfirst_serial --dataset=./datasets/five_d.txt --cities=5 --outfile=depthfirst_serial_5.txt`

The logs are generated in the mentioned output file.

### Sample command line for Serial (DFS Recursive) Algorithm

`./depthfirst_serial_recursion_dfs --dataset=./datasets/five_d.txt --cities=5`


It does not generate any log and dumps everything on STDOUT

### Sample command line for Parallel OMP Algorithm

```bash
# our results prove that for a n city database, we need atleast (n-1) threads to get the maximum speedup
export OMP_NUM_THREADS=4

# The logs are generated in the mentioned outfile. But it also has a default value.
./depthfirst_parallel --dataset=./datasets/five_d.txt --cities=5 --outfile=depthfirst_parallel_5.txt
```

### Sample command line for executing the Parallel OMP Algorithm with different configurations

```bash
# the first argument is binary of the algorithm
# the second argument is the #city dataset to use (5-21). Defaults to 15
# It changes the OMP_NUM_THREADS each time and collates the results
./run_parallel.sh depthfirst_parallel 5
```


### Sample command line for executing the Hybrid Algorithm


These are run using sbatch scripts with a sample script given below-
```bash
#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH --job-name=depthfirst_hybrid_n4_c1_i1
#SBATCH -N 1
#SBATCH -n 4
#SBATCH --time=0-00:30# time (D-HH:MM) 
#SBATCH --output=depthfirst_hybrid_n4_c1_i1_%j.out
#SBATCH -e %j.err

export OMP_NUM_THREADS=1
export MV2_ENABLE_AFFINITY=0
ibrun --npernode 4 ./depthfirst_hybrid --dataset=./datasets/five_d.txt --cities=5 --outfile=depthfirst_hybrid_n4_c1_i1_23997682
```

The above script runs 5 city dataset on 1 NODE with 4 MPI TASKS and 1 OMP THREAD each.
This particular dataset can be run with either - 
* --nodes=1, --ntasks-per-node=4, OMP_NUM_THREADS=1
* --nodes=1, --ntasks-per-node=6, OMP_NUM_THREADS=2
* --nodes=1, --ntasks-per-node=1, OMP_NUM_THREADS=2
* --nodes=1, --ntasks-per-node=4, OMP_NUM_THREADS=4

And like wise for other datasets. To get the batch scripts of all combinations automatically generated, use the below script with `--dryrun 1`

### Sample command line for running the hybrid algorithm with different configurations

This script will internally launch sbatch jobs with the identified configurations and wait until all jobs are finished to display the summary.
The help option shows - 
```bash
usage: regress_hybrid.py [-h] --executable EXECUTABLE [EXECUTABLE ...]
                         [--cities CITIES] [--iteration [ITERATION]]
                         [--dryrun [DRYRUN]]

Optional app description

optional arguments:
  -h, --help            show this help message and exit
  --executable EXECUTABLE [EXECUTABLE ...]
                        Executable to use
  --cities CITIES       #city Dataset to use
  --iteration [ITERATION]
                        Iterations per configuration, default:5
  --dryrun [DRYRUN]     Dry run. Generate scripts but do not launch, Can be 0
                        or 1. default:0

```

Note - This script needs to be run on an interactive or compute node (with ntask=1).

#### Option 1 - Interactive node

```bash
# To go to interactive node
srun --pty --nodes=1 --ntasks-per-node=1 -p debug -t 00:30:00 --wait 0 /bin/bash

module load python/2.7.10
python3 regress_hybrid.py --executable depthfirst_hybrid --cities 5
```

#### Option 2 - run using non-interactive method. 
	Use the following batch script
```bash
#!/bin/bash
#SBATCH -p compute # partition (queue)
#SBATCH -N 1 # number of nodes
#SBATCH -n 1 # number of cores
#SBATCH -t 0-10:00 # time (D-HH:MM)
#SBATCH --job-name=regress_hybrid_5
#SBATCH -o %j.out # STDOUT
#SBATCH -e %j.err # STDERR

# mail alert at start, end and abortion of execution
#SBATCH --mail-type=ALL

module load python/2.7.10

cd PHY_F244_travelling_salesman/
python3 regression.py --executable depthfirst_hybrid --cities=5
```
