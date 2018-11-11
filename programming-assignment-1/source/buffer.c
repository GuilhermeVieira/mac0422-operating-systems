// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "buffer.h"

/* Adiciona a string do Buffer B no vetor de strings parsed_command de tamanho
 * n. */
char **addToParcomm(char **parsed_command, Buffer *B, int *n)
{
    addToBuffer(B, '\0');
    // Aloca mais espaço para o vetor parsed_command se necessário.
    if ((*n)*sizeof(char*) >= sizeof(parsed_command))
        parsed_command = erealloc (parsed_command, (*n)*2*(sizeof(char *)));
    // Copia a string do Buffer para parsed_command[*n]
    parsed_command[*n] = emalloc(B->top*sizeof(char));
    parsed_command[*n] = strncpy(parsed_command[*n], B->word,
                                B->top*sizeof(char));
    (*n)++;
    clearBuffer(B); // O Buffer deve ser ter seu tamanho "resetado".
    return parsed_command;
}

/* Recebe o comando dado para o shell e um ponteiro para int que irá armazenar
 * o numero de argumentos. A função separa os argumentos e os armazena em um
 * vetor de strings que é devolvido para que o comando possa ser executado. */
char **parseCommand(char *command, int *n)
{
    char **parsed_command;
    Buffer *B;
    *n = 0;
    B = createBuffer();
    parsed_command = emalloc(sizeof(char*));
    // Quebra o comando original em argumentos.
    for (int i = 0; command[i] != '\0'; i++) {
        if (isspace(command[i])) // O espaço indica o final de um argumento.
            parsed_command = addToParcomm(parsed_command, B, n);
        else
            addToBuffer(B, command[i]);
    }
    // Adiciona o último argumento que ainda não foi adicionado.
    parsed_command = addToParcomm(parsed_command, B, n);
    // Aloca mais espaço para o vetor parsed_command se necessário.
    if ((*n)*sizeof(char*) >= sizeof(parsed_command)) {
        parsed_command = erealloc(parsed_command, (*n)*2*(sizeof(char *)));
    }
    // Coloca um ponteiro para NULL no último comando (requisito p/ execvp()).
    parsed_command[*n] = '\0';
    (*n)++;
    destroyBuffer(B);
    return parsed_command;
}


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
