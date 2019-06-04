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

More datasets - https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html
