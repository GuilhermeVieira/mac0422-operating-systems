// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include <stdio.h> // printf(), fprintf().
#include <stdlib.h> // EXIT_FAILURE, free(), sizeof().
#include <string.h> // strcmp(), strlen(), strncpy().
#include <unistd.h> // pid_t, fork(), execvp().
#include <sys/types.h> // pid_t.
#include <sys/wait.h>  // waitpid().
#include <readline/readline.h> // readline().
#include <readline/history.h> // add_history().
#include <errno.h> // int errno.
#include "buffer.h" // Funções de um buffer simples, emalloc(), erealloc().
#include "embdCmds.h" // Implementa os comandos embutidos do shell.

#define DIRECTORY_SIZE 8; // Tamanho da string diretório.

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
