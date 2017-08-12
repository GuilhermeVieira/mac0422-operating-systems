#include<stdio.h>
#include<stdlib.h>

#include<time.h> //tm, ctime, localtime

#include<string.h> //strcmp. strdup, strlen

#include<ctype.h> //isspace

#include<unistd.h> //fork e execlp

#include <sys/types.h> //waitpid
#include <sys/wait.h>  //waitpid

#include <readline/readline.h> //readline
#include <readline/history.h> //readline

#include "Buffer.h"

//precisa resolver o tamanho dessas coisas
char **parseCommand(char *command)
{
  int i, j;
  char **parsedCommand, **bob;
  Buffer *B;
  i = j = 0;
  B = criaBuffer();
  while(command[i] != '\0'){
    if(isspace(command[i])){
      printf("oi\n");
      if(sizeof(parsedCommand) < (j + 1)*sizeof(char*)){
        bob = malloc(sizeof(parsedCommand)*2);
        for(int k = 0; k < j; k++) bob[k] = parsedCommand[k];
        parsedCommand = bob;
        free(bob);
      }
      printf("ciao\n");
      parsedCommand[j] = malloc(B->top*sizeof(char));
      parsedCommand[j] = strncpy(parsedCommand[j], B->palavra, B->top);
      destroiBuffer(B);
      B = criaBuffer();
      j++;
    }
    else adicionaNoBuffer(B, command[i]);
    i++;
  }
  destroiBuffer(B);
  return parsedCommand;
}

void date()
{
  char *Date;
  //char weekDay[4], month[4];
  struct tm *clock;
  time_t epochtime;
  time(&epochtime);
  Date = ctime(&epochtime);  //vai estar em ingles
  for(int i = 0; i < strlen(Date) - 5; i++){
    printf("%c", Date[i]);
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
  pid_t child;
  char *command;
  char **parsedCommand;
  while(1){
    command = readline("$ ");
    add_history(command);
    parsedCommand = parseCommand(command);
    /*
    if(!(child = fork())){
      if(!strcmp(command, "date")) date();
      else execlp(command, command,(char*)0);
      return 0;
    }
    waitpid(child, 0, 0);
  */
  }
  return 0;
}
