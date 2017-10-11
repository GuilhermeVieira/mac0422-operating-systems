// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __TRACK_H__
#define __TRACK_H__

#include <stdlib.h> // malloc().
#include <stdio.h>  //fprintf().


typedef unsigned int uint;
typedef struct { int x, y;} position;

#define LANES 10

pthread_mutex_t track_mutex;
uint **pista;

void *emalloc(size_t size);

uint **initializeTrack(uint d, uint n);

void updateTrack(position *pos, position *old_pos, uint tag);

void destroyTrack(uint **track, uint d);

#endif
