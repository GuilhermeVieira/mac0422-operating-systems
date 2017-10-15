// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __TRACK_H__
#define __TRACK_H__

#include <stdlib.h> // malloc().
#include <stdio.h>  //fprintf().
#include <pthread.h> //mutexes
#include "globals.h"

#define LANES 10

typedef struct {
    uint pos;
    pthread_mutex_t mutex;
} square;

square **pista;
pthread_mutex_t track_mutex;

/*Aloca a memória nescessária para armezenar uma matriz d X Lanes, ocupa n
 *posições com números de 1 até n e o restante com 0's. A função retorna o espaço
 *alocado.
 */
square **initializeTrack(uint d, uint n);

/*Função irá modificar o conteudo da posição indicada por pos para tag e irá
 *mudar o conteúdo da posição indicada por old_pos para 0.
 */
void updateTrack(position *pos, position *old_pos);

/*Função irá liberar a memória usada por uma pista d X LANES.
 */
void destroyTrack(square **track, uint d);

#endif
