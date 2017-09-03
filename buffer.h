// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdio.h> // printf(), fprintf().
#include <stdlib.h> // EXIT_FAILURE, malloc(), free(), sizeof().
#include <ctype.h> // isspace().
#include <string.h> // strncpy().


#define BUFFER_MAX 32;

// Declara o tipo Buffer.
typedef struct {
                char *word; // Dados.
                int max; // Tamanho do buffer.
                int top; // Primeira posição livre.
                } Buffer;

/* Adiciona a string do Buffer B no vetor de strings parsed_command de tamanho
* n. */
char **addToParcomm(char **parsed_command, Buffer *B, int *n);

/* Recebe o comando dado para o shell e um ponteiro para int que irá armazenar
 * o numero de argumentos. A função separa os argumentos e os armazena em um
 * vetor de strings que é devolvido para que o comando possa ser executado. */
char **parseCommand(char *command, int *n);

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
