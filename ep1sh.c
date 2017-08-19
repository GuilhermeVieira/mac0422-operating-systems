#include<stdio.h> //printf().
#include<stdlib.h> //realloc(), free(), sizeof().
#include<string.h> //strcmp(), strlen(), strncpy().
#include<ctype.h> //isspace().
#include<unistd.h> //pid_t, fork(), execvp().
#include<sys/types.h> //pid_t.
#include<sys/wait.h>  //waitpid().
#include<readline/readline.h> //readline().
#include<readline/history.h> //add_history().
#include"Buffer.h"
#include"embdCmds.h"

 #include <libexplain/execvp.h> //Libs to debug
 #include <errno.h>

/*esta função recebe o comando dado para o shell e um ponteiro que ira armazenar
 *o numero de argumentos. A função ira separar os argumentos e armazenalos em
 *um vetor de strings que sera devolvido para que o comando possa ser executado.
 */
char **parseCommand(char *command, int *n)
{
    char **parsed_command;
    Buffer *B;
    *n = 0;
    B = createBuffer();
    parsed_command = emalloc(sizeof(char*));
    for (int i = 0; command[i] != '\0'; i++){
        if(isspace(command[i])){//o espaço indica o final de um argumento.
            addToBuffer(B, '\0');
            if((*n)*sizeof(char*) >= sizeof(parsed_command))
                parsed_command = realloc(parsed_command, (*n)*2*(sizeof(char *)));
            parsed_command[*n] = emalloc(B->top*sizeof(char));
            parsed_command[*n] = strncpy(parsed_command[*n], B->palavra, B->top*sizeof(char));
            (*n)++;
            clearBuffer(B);//o Buffer deve ser ter seu tamanho "reiniciado".
        }
        else addToBuffer(B, command[i]);
    }
    /*o ultimo argumento ainda não foi adicionado no parsed_command neste ponto
     *da execução.
     */
    addToBuffer(B, '\0');
    //printf("%lu %lu\n", (*n)*sizeof(char*), sizeof(parsed_command));
    if((*n)*sizeof(char*) >= sizeof(parsed_command))
        parsed_command = realloc(parsed_command, (*n)*2*(sizeof(char *)));
    parsed_command[*n] = emalloc(B->top*sizeof(char));
    parsed_command[*n] = strncpy(parsed_command[*n], B->palavra, B->top*sizeof(char));
    (*n)++;

    //CORREÇÃO DO ERRO DO LS NA PRIMEIRA VEZz

    if((*n)*sizeof(char*) >= sizeof(parsed_command))
        parsed_command = realloc(parsed_command, (*n)*2*(sizeof(char *)));
    parsed_command[*n] = emalloc(sizeof(parsed_command[*n]));
    parsed_command[*n] = '\0';
    (*n)++;

    clearBuffer(B);
    destroyBuffer(B);
    return parsed_command;
}

int main()
{
    int cmds;
    pid_t child;
    char *command;
    char **parsed_command;
    while(1){
        command = readline("$ ");//imprime no terminal e recebe o input.
        add_history(command);//adiciona o historico de comandos.
        child = fork();
        if(child == -1){
            printf("there was a problem with fork(), the command was not executed\n");
            continue;
        }
        else if(!child){
            parsed_command = parseCommand(command, &cmds);
            if(!strcmp(parsed_command[0], "date"))
                embdDate();
            else if(!strcmp(parsed_command[0], "chown"))
                embdChown(parsed_command);
            else if (strcmp(parsed_command[0], "")) {
                if (execvp(parsed_command[0], parsed_command) == -1){
                    fprintf(stderr, "%s\n", explain_execvp(parsed_command[0],parsed_command));
                    printf("------------------\n");
                    int err = errno;
                    fprintf(stderr, "%s\n", explain_errno_execvp(err,parsed_command[0],parsed_command));
                    exit(EXIT_FAILURE);
                }
            }
            for(int i = 0; i < cmds; i++)
                free(parsed_command[i]);
            free(parsed_command);
            free(command);
            return 0;
        }
        waitpid(child, 0, 0);
        free(command);
    }
    return 0;
}
