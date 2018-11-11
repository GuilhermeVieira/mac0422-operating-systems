#  Guilherme Costa Vieira               Nº USP: 9790930
#  Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

CFLAGS = -Wall -Werror -g -pthread
CC = gcc

ep2 : simulator.o track.o linked_list.o sort.o globals.o
	$(CC) $? $(CFLAGS) -o $@

simulator.o : simulator.c
	$(CC) $? $(CFLAGS) -c
sort.o : sort.c
	$(CC) $? $(CFLAGS) -c
linked_list.o : linked_list.c
	$(CC) $? $(CFLAGS) -c
track.o : track.c
	$(CC) $? $(CFLAGS) -c
globals.o : globals.c
	$(CC) $? $(CFLAGS) -c
.PHONY :
clean :
	rm *.o
	rm ep2
