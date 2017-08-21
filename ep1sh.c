// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include <stdio.h> // printf(), fprintf().
#include <stdlib.h> // EXIT_FAILURE, free(), sizeof().
#include <string.h> // strcmp(), strlen(), strncpy().
#include <ctype.h> // isspace().
#include <unistd.h> // pid_t, fork(), execvp().
#include <sys/types.h> // pid_t.
#include <sys/wait.h>  // waitpid().
#include <readline/readline.h> // readline().
#include <readline/history.h> // add_history().
#include <errno.h> // int errno.
#include "buffer.h" // Funções de um buffer simples, emalloc(), erealloc().
#include "embdCmds.h" // Implementa os comandos embutidos do shell.

#define DIRECTORY_SIZE 8; // Tamanho da string diretório.

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


char *getDirectory()
{
    int size = DIRECTORY_SIZE;
    char *prompt, *temp, *directory;
    temp = emalloc(size*sizeof(char));
    directory = getcwd(temp, size); // Devolve string com o diretório.
    /* Caso DIRECTORY_SIZE não seja suficiente, realoca até conseguir pegar
     *  o diretório. */
    while (directory == NULL && errno == ERANGE) {
        size *= 2;
        temp = erealloc(temp, size*sizeof(char));
        directory = getcwd(temp, size);
    }
    prompt = emalloc((5 + strlen(directory))*sizeof(char));
    prompt = strcpy(prompt, "[\0");
    prompt = strcat(prompt, directory);
    prompt = strcat(prompt, "]$ \0");
    free(temp);
    return prompt;
}

int main()
{
    int cmds;
    char *command, *prompt;
    char **parsed_command;
    pid_t child;

    // Loop infinito do shell.
    while (1) {
        prompt = getDirectory(); // Imprime o diretório atual e o símbolo $.
        command = readline(prompt);
        free(prompt);
        add_history(command); // Adiciona o histórico de comandos no shell.

        // Realiza a syscall fork() para criar o processo filho.
        child = fork();
        // Verifica se o processo filho foi criado com sucesso.
        if (child == -1) {
            fprintf(stderr,
                    "ERRO: Não foi possível executar o comando.\n");
            continue;
        }
        // Verifica se este é o processo filho.
        else if (!child) {
            // "Parsea" o comando digitado pelo usuário
            parsed_command = parseCommand(command, &cmds);
            // Verifica se o comando é o "date" (embutido) e o executa.
            if (!strcmp(parsed_command[0], "date"))
                embdDate();
            // Verifica se o comando é o "chown" (embutido)e o executa.
            else if (!strcmp(parsed_command[0], "chown"))
                embdChown(parsed_command);
            // Verifica se o comando é externo e o executa.
            else if (strcmp(parsed_command[0], "")) {
                // Executa a syscall execvp() e verifica se teve sucesso.
                if (execvp(parsed_command[0], parsed_command) == -1) {
                    fprintf(stderr,
                            "ERRO: Não foi possível executar o comando.\n");
                    continue;
                }
            }
            // Libera o espaço alocado.
            for (int i = 0; i < cmds; i++)
                free(parsed_command[i]);
            free(parsed_command);
            free(command);
            return 0;
        }
        // Espera o processo filho terminar para voltar para o shell.
        waitpid(child, 0, 0);
        free(command);
    }
    return 0;
}
