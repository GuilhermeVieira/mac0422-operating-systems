#include<stdio.h> //printf()
#include<stdlib.h> //realloc(), free(), sizeof()
#include<time.h> //tm, ctime(), localtime()
#include<string.h> //strcmp(), strlen(), strncpy
#include<ctype.h> //isspace()
#include<unistd.h> //pid_t, fork(), execvp()
#include<sys/types.h> //waitpid()
#include<sys/wait.h>  //waitpid()
#include<readline/readline.h> //readline()
#include<readline/history.h> //readline()
#include<pwd.h> // passwd, getpwnam()
#include <grp.h>
#include"Buffer.h"
#include <sys/stat.h>


//precisa resolver o tamanho dessas coisas
char **parseCommand(char *command, int *n)
{
    int i = 0;
    char **parsedCommand;
    Buffer *B;
    *n = 0;
    B = createBuffer();
    parsedCommand = emalloc(sizeof(char*));
    while(command[i] != '\0'){
        if(isspace(command[i])){
            addToBuffer(B, '\0');
            if((*n)*sizeof(char*) > sizeof(parsedCommand))
                parsedCommand = realloc(parsedCommand, (*n)*2*(sizeof(char *)));
            parsedCommand[*n] = emalloc(B->top*sizeof(char));
            parsedCommand[*n] = strncpy(parsedCommand[*n], B->palavra, B->top);
            clearBuffer(B);
            (*n)++;
        }
        else addToBuffer(B, command[i]);
        i++;
    }
    addToBuffer(B, '\0');
    if((*n)*sizeof(char*) > sizeof(parsedCommand))
        parsedCommand = realloc(parsedCommand, (*n)*2*(sizeof(char *)));
    parsedCommand[*n] = emalloc(B->top*sizeof(char));
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
    int i, j;
    struct stat *groupData, *userData;
    Buffer *usr, *grp;
    groupData = emalloc(sizeof(struct stat));
    userData = emalloc(sizeof(struct stat));
    usr = createBuffer();
    grp = createBuffer();
    i = 0;
    while(commands[1][i] != ':'){
        addToBuffer(usr, commands[1][i]);
        i++;
    }
    if(!usr->top) userData->st_uid = -1;
    else stat(usr->palavra, userData);
    i++;
    for(j = i; j < strlen(commands[1]); j++) addToBuffer(grp,commands[1][j]);
    if(!grp->top) groupData->st_gid = -1;
    else stat(grp->palavra, groupData);
    chown(commands[2], userData->st_uid, groupData->st_gid);
    free(groupData);
    free(userData);
    destroyBuffer(usr);
    destroyBuffer(grp);
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
        child = fork();
        if(child == -1){
            printf("there was a problem with fork(), the command was not executed\n");
            continue;
        }
        else if(!child){
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
