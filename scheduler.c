#include <string.h> //strcmp()
#include <stdlib.h> //atoi()
#include <time.h>  //nanosleep()
#include <unistd.h> //sleep()
#include <pthread.h>
#include <math.h> //pow()
#include "fileHandlerr.h"

#define QUANTUM 0.5
#define NAP_TIME 0.5

pthread_mutex_t mutex;
double clock_time = 0;
int cs_counter = 1;

void nap(double dt)
{
    double temp;
    int temp2 = dt;
    temp = dt - temp2;
    temp *= 10;
    nanosleep((const struct timespec[]){{temp2, temp*100000000L}}, NULL);
    clock_time += dt;
    return;
}

void *simulateProcSJF(void *toSchedule)
{
    nap(((List)toSchedule)->info->dt);
    return NULL;
}

List getNextShortest(List root)
{
    List shortest = root;
    for (;root != NULL; root = root->next){
        if (shortest->info->dt > root->info->dt)
            shortest = root;
    }
    return shortest;
}

void SJF(char *inputFile, char *outputFile, int optional)
{
    printf("%d\n", optional);
    pthread_t thread;
    List toArrive, shortest, toSchedule = NULL;
    double temp;
    toArrive = readFile(inputFile);
    while((toSchedule != NULL || toArrive != NULL)){
        toSchedule = add(toSchedule, &toArrive, clock_time, optional);
        printf("%f\n", clock_time);
        if (toSchedule == NULL){
            nap(1);
            continue;
        }
        shortest = getNextShortest(toSchedule);
        if (optional == 1)
            fprintf(stderr, "O processo %s começou usar a CPU %d\n", shortest->info->name, 1);
        pthread_create(&thread, NULL, &simulateProcSJF, (void *)shortest);
        pthread_join(thread, NULL);
        if (optional == 1)
            fprintf(stderr, "O processo %s deixou de usar a CPU %d\n", shortest->info->name, 1);
        cs_counter++;
        if (optional == 1)
            fprintf(stderr, "Mudança de contexto %d\n", cs_counter);
        writeFile(outputFile, shortest->info, clock_time, optional);
        toSchedule = removeList(toSchedule, shortest->info);
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

int calcPriority(Process *proc)
{
    double priority = -pow(((clock_time - proc->deadline)/proc->dt), 2) + 10;
    if (priority > 1) {
    	if ((int)(priority + 1) < 5 && clock_time > proc->deadline) return 5;
    	return (int)(priority + 1);
    }
    return 1;
}

void *simulateProcPRR(void *proc){
	int priority;
    Process *procc = (Process *)proc;
    while (procc->dt != procc->run_time){
        pthread_mutex_lock(&(procc->mutex));
        printf("estou rodando o processo %s\n", procc->name);
        priority = calcPriority(procc);
        printf("PRIORIDADE %d\n", priority);
        if (procc->dt - procc->run_time >= QUANTUM*priority){
            nap(QUANTUM*priority);
            procc->run_time += QUANTUM*priority;
        }
        else{
            nap(procc->dt - procc->run_time);
            procc->run_time += procc->dt - procc->run_time; //mesma coisa de fazer proc->run_time = proc->dt;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void roundRobin(char *inputFile, char *outputFile, int optional, int scheduler_type)
{
    List toArrive, toSchedule = NULL;
    List currProcess = NULL;
    toArrive = readFile(inputFile);
    pthread_mutex_init(&mutex, NULL);
    while(toSchedule != NULL || toArrive != NULL){
        toSchedule = add(toSchedule, &toArrive, clock_time, optional);
        if (toSchedule == NULL){
            nap(NAP_TIME);
            continue;
        }
        if (currProcess == NULL) // Faz a lista ser circular
           currProcess = toSchedule;
        //ver se ela já rodou, se não iniciar farol;
        if (currProcess->info->run_time == 0){
            pthread_mutex_init(&(currProcess->info->mutex), NULL);
            if (optional == 1)
                fprintf(stderr, "O processo %s começou usar a CPU %d\n", currProcess->info->name, 1);
            if (scheduler_type == 2)
                pthread_create(&(currProcess->info->thread), NULL, &simulateProcRR, currProcess->info);
            else
                pthread_create(&(currProcess->info->thread), NULL, &simulateProcPRR, currProcess->info);
        }
        else{
            if (optional == 1)
                fprintf(stderr, "O processo %s começou usar a CPU %d\n", currProcess->info->name, 1);
            pthread_mutex_unlock(&(currProcess->info->mutex));
        }
        pthread_mutex_lock(&mutex);
        cs_counter++;
        if (optional == 1){
            fprintf(stderr, "O processo %s deixou de usar a CPU %d", currProcess->info->name, 1);
            fprintf(stderr, "Mudança de contexto %d", cs_counter);
        }
        if (currProcess->info->run_time >= currProcess->info->dt){
            writeFile(outputFile, currProcess->info, clock_time, optional);
            pthread_mutex_destroy(&(currProcess->info->mutex));
            pthread_cancel(currProcess->info->thread);
            List temp = currProcess->next;
            toSchedule = removeList(toSchedule, currProcess->info);
            currProcess = temp;
            continue;
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
    FILE *fp;
    if (argc >= 5 && !strcmp("d", argv[4]))
        optional = 1;
    if (scheduler_type == 1)
        SJF(inputFile, outputFile, optional);
    else
        roundRobin(inputFile, outputFile, optional, scheduler_type);

    fp = fopen(outputFile, "a");
    fprintf(fp, "%d\n", cs_counter);
    fclose(fp);
    return 0;
}
