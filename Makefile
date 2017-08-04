#CC = ../dvsdk_ipnctools/linux-devkit/bin/arm-arago-linux-gnueabi-gcc
CC = gcc
CFLAGS = -W -WALL

all: 
	$(CC) rs232.c -o rs232
	$(CC) rs485.c -o rs485	

clean:	
	@rm -rf rs232
	@rm -rf rs485




