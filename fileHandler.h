// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __FILEHANDLER_H__
#define __FILEHANDLER_H__

#include <stdio.h>  //fopen(), fprintf(), getline()
#include <string.h> //strcmp()
#include <stdlib.h> //atof()
#include "buffer.h"

typedef struct {
                int line; // Linha do arquivo trace
                double t0, dt, deadline, run_time;
                char *name;
                pthread_mutex_t mutex;
                pthread_t thread;
                } Process;

typedef struct node {
                    Process *info;
                    struct node *next;
                    } Cell;

// Apontador para o ínicio da lista de processos.
typedef Cell *List;

int cmpfunc(const void *a, const void *b);

void writeFile(char *outputFile, Process *proc, double time, int optional);

List readFile(char *fileName);

List createList();

List addList(List root, Process *new_process);

List removeList(List root, Process *x);

void destroyProcess(Process *x);

Process *createProcess(double t0, double dt, double dl, char *name, int line);

List add(List bob, List *bob2, double time, int optional);//

List getTail(List head);


#endif
