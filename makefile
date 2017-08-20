CFLAGS = -Wall -Werror -g -lreadline -lexplain
CC = gcc

ep1sh: ep1sh.o Buffer.o embdCmds.o
	$(CC) $? $(CFLAGS) -o $@ -I.
ep1sh.o: ep1sh.c Buffer.h embdCmds.h
	$(CC) ep1sh.c $(CFLAGS) -c
embdCmds.o: embdCmds.c embdCmds.h Buffer.h
	$(CC) embdCmds.c $(CFLAGS) -c
Buffer.o: Buffer.c Buffer.h
	$(CC) Buffer.c $(CFLAGS) -c
clean:
	$(RM) count *.o *~
