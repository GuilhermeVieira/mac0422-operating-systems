#include <string.h> //strcmp()
#include <stdlib.h> //atoi()
#include <time.h>  //nanosleep()
#include <unistd.h> //sleep()
#include <pthread.h>
#include "fileHandlerr.h"

#define QUANTUM 1.5

pthread_mutex_t mutex;

void nap(double dt)
{
    double temp;
    int temp2 = dt;
    temp = dt - temp2;
    temp *= 10;
    nanosleep((const struct timespec[]){{temp2, temp*100000000L}}, NULL);
    return;
}

void *simulateProcSJF(void *toSchedule)
{
    nap(((List)toSchedule)->info->dt);
    return NULL;
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
        writeFile(outputFile, toSchedule->info, time);
        toSchedule = removeList(toSchedule, toSchedule->info);
    }
    return;
}

void *simulateProcRR(void *proc){
    Process *procc = (Process *)proc;
    while (procc->dt != procc->run_time){
        pthread_mutex_lock(&(procc->mutex));
        printf("estou rodando o processo %s\n", procc->name);
        if (procc->dt - procc->run_time >= QUANTUM){
            nap(QUANTUM);
            procc->run_time += QUANTUM;
        }
        else{
            nap(procc->dt - procc->run_time);
            procc->run_time += procc->dt - procc->run_time; //mesma coisa de fazer proc->run_time = proc->dt;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void roundRobin(char *inputFile, char *outputFile, int optional)
{
    List toArrive, toSchedule = NULL;
    List currProcess = NULL;
    double time = 0;
    toArrive = readFile(inputFile);
    pthread_mutex_init(&mutex, NULL);
    while(toSchedule != NULL || toArrive != NULL){
        toSchedule = add(toSchedule, &toArrive, time);
        if (toSchedule == NULL){
            nap(0.5);
            time += 0.5;
            //pthread_mutex_unlock(&mutex);  //num sei se isso ta certo de estar aqui;
            continue;
        }
        if (currProcess == NULL)
           currProcess = toSchedule;
        //ver se ela já rodou, se não iniciar farol;
        if (currProcess->info->run_time == 0){
            pthread_mutex_init(&(currProcess->info->mutex), NULL);
            pthread_create(&(currProcess->info->thread), NULL, &simulateProcRR, currProcess->info);
        }
        else
            pthread_mutex_unlock(&(currProcess->info->mutex));
        pthread_mutex_lock(&mutex);   //num sei onde colocar isso;
        time += QUANTUM;
        if (currProcess->info->run_time >= currProcess->info->dt){
            writeFile(outputFile, toSchedule->info, time);
            pthread_mutex_destroy(&(currProcess->info->mutex));
            pthread_cancel(currProcess->info->thread);
            toSchedule = removeList(toSchedule, currProcess->info); //isso não parece certo não
        }
        currProcess = currProcess->next;
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
    if (scheduler_type == 1)
        SJF(inputFile, outputFile, optional);
    else if (scheduler_type == 2)
        roundRobin(inputFile, outputFile, optional);
    return 0;
}
