CC=gcc
OPTS=-std=c99 -Werror

all: depthfirst_serial depthfirst_parallel depthfirst_serial_recursion_dfs

depthfirst_serial: 
	$(CC) $(OPTS) -o depthfirst_serial depthfirst_serial.c -lm

depthfirst_serial_recursion_dfs:
	$(CC) $(OPTS) -o depthfirst_serial_recursion_dfs  depthfirst_serial_recursion_dfs.c -lm

depthfirst_parallel: 
	$(CC) $(OPTS) -fopenmp -o depthfirst_parallel depthfirst_parallel.c -lm

clean: 
	rm -f depthfirst_serial depthfirst_serial_recursion_dfs depthfirst_parallel;
