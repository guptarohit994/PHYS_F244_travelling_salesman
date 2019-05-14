CC=gcc
OPTS=-g -std=c99 -Werror

depthfirst_serial: 
	$(CC) $(OPTS) -o depthfirst_serial depthfirst_serial.c

clean: 
	rm -f depthfirst_serial;
