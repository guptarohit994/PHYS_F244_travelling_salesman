CC=gcc
OPTS=-g -std=c99 -Werror

depthfirst_serial: 
	$(CC) -o depthfirst_serial depthfirst_serial.c

clean: 
	rm -f depthfirst_serial;
