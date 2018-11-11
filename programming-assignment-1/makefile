CFLAGS = -Wall -Werror -g -lreadline -pthread -lm -O3
CC = gcc

all: ep1 ep1sh

ep1: scheduler.o buffer.o fileHandler.o
	$(CC) $? $(CFLAGS) -o $@ -I.
ep1sh: ep1sh.o buffer.o embdCmds.o
	$(CC) $? $(CFLAGS) -o $@ -I.
ep1sh.o: buffer.h embdCmds.h
	$(CC) ep1sh.c $(CFLAGS) -c
embdCmds.o: embdCmds.h buffer.h
	$(CC) embdCmds.c $(CFLAGS) -c
fileHandler.o: fileHandler.h buffer.h
	$(CC) fileHandler.c $(CFLAGS) -c
buffer.o: buffer.h
	$(CC) buffer.c $(CFLAGS) -c
.PHONY: clean
clean:
	-rm *.o
