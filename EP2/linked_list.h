// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __LINKED_LIST_H__
#define __LINKED_LISR_H__

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

typedef struct node { uint lap, vector_size, top; uint *tags_vector; struct node *next; } Cell;
typedef Cell *List;

/*Função cria uma lista.
 */
List createList();

/*Função adiciona a tag no vetor pertencente ao node da lista ligada representado
 *por lap, se não existir o node é criado. A função devolve o começo da lista.
 */
List addList(List root, uint lap, uint tag);

/*Remove o primeiro node da lista, desaloca sua mémoria. A função devolve o novo
 *começo da lista.
 */
List removeList(List root);

/*Função ira imprimir na saída padrão o vetor presente no primeiro node da lista
 *e ira chamar a removeList(). A função retorna o retorno da removeList().
 */
List printLap(List root);

#endif
