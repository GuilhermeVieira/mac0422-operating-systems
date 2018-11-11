// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __SORT_H__
#define __SORT_H__

#include <stdlib.h>
#include "globals.h"

/*Função de comparação para o qsort() da stdlib. Usa o campo laps da struct rank
 *como critério de comparação.
 */
int cmpLaps(const void *a, const void *b);

/*Função de comparação para o qsort() da stdlib. Usa o campo pos da struct rank
 *como critério de comparação.
 */
int cmpPos(const void *a, const void *b);

/*Função de comparação para o qsort() da stdlib. Usa o campo pts da struct rank
 *como critério de comparação.
 */
int cmpPts(const void *a, const void *b);

/*Função usada para ordenar somente a parte delimitada por beginning e end do
 *vetor array. Usa o campo pos da struct rank como critério.
 */
void sort_range_array(rank *array, int beginning, int end);

#endif
