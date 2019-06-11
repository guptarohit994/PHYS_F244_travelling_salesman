CC=gcc
MPICC=mpicc
OPTS=-std=c99 -Werror -D_POSIX_SOURCE

all: depthfirst_serial depthfirst_parallel depthfirst_serial_recursion_dfs depthfirst_hybrid

depthfirst_serial: 
	$(CC) $(OPTS) -fopenmp -o depthfirst_serial depthfirst_serial.c -lm

depthfirst_serial_recursion_dfs:
	$(CC) $(OPTS) -fopenmp -o depthfirst_serial_recursion_dfs  depthfirst_serial_recursion_dfs.c -lm

depthfirst_parallel: 
	$(CC) $(OPTS) -fopenmp -o depthfirst_parallel depthfirst_parallel.c -lm

tsp_greedy:
	$(CC) $(OPTS) -o tsp_greedy tsp_greedy.c -lm

depthfirst_hybrid:
	$(MPICC) $(OPTS) -fopenmp -o depthfirst_hybrid depthfirst_hybrid.c -lm

clean: 
	rm -f depthfirst_serial depthfirst_serial_recursion_dfs depthfirst_parallel depthfirst_hybrid;
