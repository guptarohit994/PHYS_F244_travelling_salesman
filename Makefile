CC=gcc
OPTS=-std=c99 -Werror

depthfirst_serial: 
	$(CC) $(OPTS) -o depthfirst_serial depthfirst_serial.c

depthfirst_serial_recursion_dfs:
	$(CC) $(OPTS) -o depthfirst_serial_recursion_dfs  depthfirst_serial_recursion_dfs.c

depthfirst_parallel: 
	$(CC) $(OPTS) -o depthfirst_parallel depthfirst_parallel.c

clean: 
	rm -f depthfirst_serial depthfirst_serial_recursion_dfs depthfirst_parallel;
