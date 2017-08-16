#include<stdio.h> //printf()
#include<stdlib.h> //malloc(), realloc(), free(), sizeof()
#include<time.h> //tm, ctime(), localtime()
#include<string.h> //strcmp(), strlen(), strncpy
#include<ctype.h> //isspace()
#include<unistd.h> //pid_t, fork(), execvp()
#include<sys/types.h> //waitpid()
#include<sys/wait.h>  //waitpid()
#include<readline/readline.h> //readline()
#include<readline/history.h> //readline()
#include <pwd.h>
#include"Buffer.h"



//precisa resolver o tamanho dessas coisas
char **parseCommand(char *command, int *n)
{
    int i = 0;
    char **parsedCommand;
    Buffer *B;
    *n = 0;
    B = createBuffer();
    parsedCommand = malloc(sizeof(char*));
    while(command[i] != '\0'){
        if(isspace(command[i])){
            if((*n)*sizeof(char*) > sizeof(parsedCommand))
                parsedCommand = realloc(parsedCommand, (*n)*2*(sizeof(char *)));
            parsedCommand[*n] = malloc(B->top*sizeof(char));
            parsedCommand[*n] = strncpy(parsedCommand[*n], B->palavra, B->top);
            clearBuffer(B);
            (*n)++;
        }
        else addToBuffer(B, command[i]);
        i++;
    }
    if((*n)*sizeof(char*) > sizeof(parsedCommand))
        parsedCommand = realloc(parsedCommand, (*n)*2*(sizeof(char *)));
    parsedCommand[*n] = malloc(B->top*sizeof(char));
    parsedCommand[*n] = strncpy(parsedCommand[*n], B->palavra, B->top);
    clearBuffer(B);
    (*n)++;
    destroyBuffer(B);
    return parsedCommand;
}

void date()
{
    char *dayInfo;
    struct tm *clock;
    time_t epochtime;
    time(&epochtime);
    dayInfo = ctime(&epochtime);
    for(int i = 0; i < strlen(dayInfo) - 5; i++){
        printf("%c", dayInfo[i]);
    }
    clock = localtime(&epochtime);
    printf("%s %d\n", clock->tm_zone, clock->tm_year + 1900);
    return;
}

void Chown(char **commands)
{
    struct passwd *userData;
    char *grp;
    userData = getpwnam(commands[2]);
    grp = malloc((strlen(commands[1])-1)*sizeof(char));
    for(int i = 1, j = 0; i < strlen(commands[1]); i++, j++) grp[j] = commands[1][i];
    chown(commands[0],-1,userData->pw_gid);
    return;
}

int main()
{
    int cmds;
    pid_t child;
    char *command;
    char **parsedCommand;
    while(1){
        command = readline("$ ");
        add_history(command);
        if(!(child = fork())){
            parsedCommand = parseCommand(command, &cmds);
            if(!strcmp(parsedCommand[0], "date")) date();
            else if(!strcmp(parsedCommand[0], "chown")) Chown(parsedCommand);
            else execvp(parsedCommand[0], parsedCommand);
            for(int i = 0; i < cmds; i++) free(parsedCommand[i]);
            free(parsedCommand);
            return 0;
        }
        waitpid(child, 0, 0);
    }
    return 0;
}
