// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "buffer.h"

Buffer *createBuffer()
{
    Buffer *B = emalloc(sizeof(Buffer));
    B->max = BUFFER_MAX;
    B->top = 0;
    B->word = emalloc(B->max*sizeof(char));
    return B;
}

void reallocBuffer(Buffer *B)
{
    int i;
    char *Temp;
    Temp = emalloc(2*B->max*sizeof(char));
    for (i = 0; i < B->max; i++)
        Temp[i] = B->word[i];
    free(B->word);
    B->max = 2*B->max;
    B->word = Temp;
    return;
}

void addToBuffer(Buffer *B, char c)
{
    // Verifica se o buffer está cheio.
    if (B->top == B->max)
        reallocBuffer(B);
    // Adiciona o char c no buffer.
    B->word[B->top] = c;
    B->top++;
    return;
}

void destroyBuffer(Buffer *B)
{
    free(B->word);
    free(B);
    return;
}

void clearBuffer(Buffer *B)
{
    B->top = 0;
    return;
}

void *emalloc(size_t size)
{
    void *pointer;
    pointer = malloc(size);
    if (pointer == NULL) {
        fprintf(stderr,
                "ERRO: Não foi possível alocar memória suficiente.\n");
        exit(EXIT_FAILURE);
    }
    else return pointer;
}

void *erealloc(void *pointer, size_t size)
{
    pointer = realloc(pointer, size);
    if (pointer == NULL) {
        fprintf(stderr,
                "ERRO: Não foi possível alocar memória suficiente.\n");
        exit(EXIT_FAILURE);
    }
    else return pointer;
}
