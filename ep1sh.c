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
#include"Buffer.h"

//#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>


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
    //char weekDay[4], month[4];
    struct tm *clock;
    time_t epochtime;
    time(&epochtime);
    dayInfo = ctime(&epochtime);  //vai estar em ingles
    for(int i = 0; i < strlen(dayInfo) - 5; i++){
        printf("%c", dayInfo[i]);
    }
    clock = localtime(&epochtime);
    printf("%s %d\n", clock->tm_zone, clock->tm_year + 1900);
    /* nao vai estar em ingles
    if(clock->tm_wday == 0) strcpy(weekDay, "Dom");
    else if(clock->tm_wday == 1) strcpy(weekDay, "Seg");
    else if(clock->tm_wday == 2) strcpy(weekDay, "Ter");
    else if(clock->tm_wday == 3) strcpy(weekDay, "Qua");
    else if(clock->tm_wday == 4) strcpy(weekDay, "Qui");
    else if(clock->tm_wday == 5) strcpy(weekDay, "Sex");
    else strcpy(weekDay, "Sab");
    if(clock->tm_mon == 0) strcpy(month, "Jan");
    else if(clock->tm_mon == 1) strcpy(month, "Fev");
    else if(clock->tm_mon == 2) strcpy(month, "Mar");
    else if(clock->tm_mon == 3) strcpy(month, "Abr");
    else if(clock->tm_mon == 4) strcpy(month, "Mai");
    else if(clock->tm_mon == 5) strcpy(month, "Jun");
    else if(clock->tm_mon == 6) strcpy(month, "Jul");
    else if(clock->tm_mon == 7) strcpy(month, "Ago");
    else if(clock->tm_mon == 8) strcpy(month, "Set");
    else if(clock->tm_mon == 9) strcpy(month, "Out");
    else if(clock->tm_mon == 10) strcpy(month, "Nov");
    else strcpy(month, "Dez");
    printf("%s %s %d %d:%d:%d %s %d\n", weekDay, month, clock->tm_mday, clock->tm_hour, clock->tm_min, clock->tm_sec, clock->tm_zone, clock->tm_year + 1900);
    */
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
