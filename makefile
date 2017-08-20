CFLAGS = -Wall -Werror -g -lreadline -lexplain
CC = gcc

ep1sh: ep1sh.o Buffer.o embdCmds.o
	$(CC) $? $(CFLAGS) -o $@ -I.
.INTERMEDIATE: ep1sh.o embdCmds.o Buffer.o
ep1sh.o: Buffer.h embdCmds.h
	$(CC) ep1sh.c $(CFLAGS) -c
embdCmds.o: embdCmds.h Buffer.h
	$(CC) embdCmds.c $(CFLAGS) -c
Buffer.o: Buffer.h
	$(CC) Buffer.c $(CFLAGS) -c
