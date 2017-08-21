// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdio.h> // printf(), fprintf().
#include <stdlib.h> // EXIT_FAILURE, malloc(), free(), sizeof().

#define BUFFER_MAX 32;

// Declara o tipo Buffer.
typedef struct {
                char *word; // Dados.
                int max; // Tamanho do buffer.
                int top; // Primeira posição livre.
                } Buffer;

 // Cria Buffer (vetor dinâmico) de char e devolve um ponteiro para Buffer.
Buffer *createBuffer();

// Recebe um ponteiro para Buffer e dobra seu tamanho, mantendo seu conteúdo.
void reallocBuffer(Buffer *B);

// Carrega o caractere c no Buffer B.
void addToBuffer(Buffer *B, char c);

// Libera o espaço alocado pelo Buffer B.
void destroyBuffer(Buffer *B);

// Reseta o Buffer B para seu estado inicial.
void clearBuffer(Buffer *B);

// Função malloc() com tratamento de erro.
void *emalloc(size_t size);

// Função realloc() com tratamento de erro.
void *erealloc(void *pointer, size_t size);

#endif
