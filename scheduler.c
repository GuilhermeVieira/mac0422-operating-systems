#include <string.h> //strcmp()
#include <stdlib.h> //atoi()
#include <time.h>  //nanosleep()
#include <unistd.h> //sleep()
#include <pthread.h>

#include "fileHandlerr.h"


void nap(double dt)
{
    double temp;
    int temp2 = dt;
    temp = dt - temp2;
    temp *= 10;
    nanosleep((const struct timespec[]){{temp2, temp*100000000L}}, NULL);
    return;
}

void simulateProcSJF(void *toSchedule)
{
    nap(((List)toSchedule)->info->dt);
    return;
}

void SJF(char *inputFile, char *outputFile, int optional)
{
    pthread_t thread;
    List toArrive, toSchedule = NULL;
    double time = 0, temp;
    toArrive = readFile(inputFile);
    while((toSchedule != NULL || toArrive != NULL)){
        toSchedule = add(toSchedule, &toArrive, time);
        printf("%f\n", time);
        time++;
        if (toSchedule == NULL){
            sleep(1);
            continue;
        }
        pthread_create(&thread, NULL, &simulateProcSJF, (void *)toSchedule);
        time += toSchedule->info->dt;
        pthread_join(thread, NULL);
        for(List temp = toSchedule ;temp != NULL; temp = temp->next)
            printf("%s\n", temp->info->name);
        printf("/////////\n");
        for(List temp = toArrive ;temp != NULL; temp = temp->next)
            printf("%s\n", temp->info->name);
        toSchedule = removeList(toSchedule, toSchedule->info);
    }
    return;
}

int main(int argc, char **argv)
{
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *inputFile, *outputFile;
    inputFile = argv[2];
    outputFile = argv[3];
    if (argc >= 4 && strcmp("d", argv[4]))
        optional = 1;
    SJF(inputFile, outputFile, optional);
    return 0;
}
