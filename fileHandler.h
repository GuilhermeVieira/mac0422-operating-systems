// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __FILEHANDLER_H__
#define __FILEHANDLER_H__

#include <stdio.h>  // fopen(), fprintf(), getline().
#include <string.h> // strcmp().
#include <stdlib.h> // atof(), qsort().
#include "buffer.h"

typedef struct {
                int line;
                double t0, dt, deadline, run_time;
                char *name;
                pthread_mutex_t mutex;
                pthread_t thread;
                } Process;

typedef struct node {
                    Process *info;
                    struct node *next;
                    } Cell;

typedef Cell *List;

// Usada pelo qsort() para determinar qual processo tem dt menor.
int cmpfunc(const void *a, const void *b);

/* Escreve as informações necessárias sobre um processe (proc) em um
 * arquivo de saída (output_file) e recebe um número que se for igual a
 * 1 irá imprimir informações adicionais na sterr. */
void writeFile(char *output_file, Process *proc, double time, int optional);

/* Recebe o nome de um arquivo com processos e ira passar essas informações
 * para uma lista ligada. */
List readFile(char *file_name);

// Cria a cabeça de uma lista.
List createList();

// Adiciona um processo ao final de uma lista ligada não ordenada de processos.
List addList(List root, Process *new_process);

// Remove um processo (indentificado pelo seu nome) da lista ligada fornecida.
List removeList(List root, Process *x);

// Libera a memória usada por um processo.
void destroyProcess(Process *x);

/* Cria um processo que irá ser iniciado em t0, irá rodar por dt segundos,
 * possui deadline igual a dl, um indetificador igual a name e se encontrava
 * na linha line do arquivo de trace. */
Process *createProcess(double t0, double dt, double dl, char *name, int line);

/* Recebe as lista dos processos que já chegaram to_schedule e um ponteiro
 * para a lista dos processos que estão para chegar. Verifica quais processos
 * chegaram e transfere de uma lista para a outra. */
List add(List to_schedule, List *to_arrive, double time, int optional);

// Retorna um ponteriro para a "tail" da lista ligada apontada por head.
List getTail(List head);

#endif
