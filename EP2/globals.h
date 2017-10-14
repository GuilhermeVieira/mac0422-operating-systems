// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define MS60  2
#define MS20  6

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef unsigned int uint;
typedef struct { int x, y;} position;
typedef struct {position pos; uint laps, broken_lap, pts, tag; double time_elapsed; } rank;

uint n_cyclists;
int is_over;
rank *ranking;
uint points_buffer[4];

/*Função alloca um espaço de memória e trata o póssivel erro de falta de espaço.
 */
void *emalloc(size_t size);

#endif
