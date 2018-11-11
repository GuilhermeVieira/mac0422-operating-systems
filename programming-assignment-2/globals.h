// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define MS60  2 // 60ms.
#define MS20  6 // 20ms.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef unsigned int uint;
typedef struct { int x, y;} position;
typedef struct {position pos; uint laps, broken_lap, pts, tag; double time_elapsed; } rank;

uint n_cyclists; // Número variável de ciclistas da prova.
int is_over; // Vale 1 se a corrida acabou.
rank *ranking; // Contém pontuação e a volta onde cada ciclista está.
uint points_buffer[4]; // Buffer que guarda quem ganhou pontos.

// Aloca um espaço de memória e trata o possível erro de falta de espaço.
void *emalloc(size_t size);

#endif
