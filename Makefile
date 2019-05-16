CC=gcc
OPTS=-g -std=c99 -Werror

depthfirst_serial: 
	$(CC) $(OPTS) -o depthfirst_serial depthfirst_serial.c

depthfirst_serial_recursion_dfs:
	$(CC) $(OPTS) -o depthfirst_serial_recursion_dfs  depthfirst_serial_recursion_dfs.c

clean: 
	rm -f depthfirst_serial depthfirst_serial_recursion_dfs;