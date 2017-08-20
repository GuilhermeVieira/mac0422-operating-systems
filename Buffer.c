#include <stdio.h>   //printf().
#include <stdlib.h> //EXIT_FAILURE, malloc(), free(), sizeof().
#include "Buffer.h"

Buffer *createBuffer()
{
    Buffer *B = emalloc(sizeof(Buffer));
    B->max = 8;
    B->top = 0;
    B->palavra = emalloc(B->max*sizeof(char));
    return B;
}

void reallocBuffer(Buffer *B)
{
    int i;
    char *Temp;
    Temp = emalloc(2*B->max*sizeof(char));
    for (i = 0; i < B->max; i++)
        Temp[i] = B->palavra[i];
    free(B->palavra);
    B->max = 2*B->max;
    B->palavra = Temp;
    return;
}

void addToBuffer(Buffer *B, char c)
{
    /*antes de adicionadar no buffer presisamos ver se ele esta cheio.
     */
    if (B->top == B->max)
        reallocBuffer(B);
    B->palavra[B->top] = c;
    B->top++;
    return;
}

void destroyBuffer(Buffer *B)
{
    free(B->palavra);
    free(B);
    return;
}

void clearBuffer(Buffer *B)
{
    /*para "resetar" o buffer só é nescessario mudar o seu topo para 0.
     */
    B->top = 0;
    return;
}

void *emalloc(size_t size)
{
    void *pointer;
    pointer = malloc(size);
    if (pointer == NULL) {
        fprintf (stderr, "There was an error with the malloc function\n");
        exit(EXIT_FAILURE);
    }
    else return pointer;
}
