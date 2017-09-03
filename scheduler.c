#include <stdio.h>
#include <stdlib.h> //atoi()
#include <string.h> //strcmp()
#include <pthread.h> //threads
#include <time.h> //nanosleep()
#include <unistd.h> //sysconf()

void *roundRobin(bob *toArrive, int optional)
{
    unsigned double time = 0.0;
    bob *haveToSchedule = creatBob();
    pthread_t *threads;
    pthread_mutex_t *mutex;
    while (haveToSchedule->top == 0 && toArrive->top == 0){
        bob *new_process =  getNewProcess(toArrive, time);
        for (int i = 0; i < new_process->top; i++)
            addProcess(haveToSchedule, new_process->process_list[i]);
        if (haveToSchedule->top == 0){
            nanosleep(0, 100000000L);
            time += 0.1;
        }
        else{
            pthread_mutex_lock(mutex[0]);
            threads = erealloc(threads, haveToSchedule->top);
            mutex = erealloc(mutex, haveToSchedule->top + 1);
            for (int i = haveToSchedule->top - new_process->top; i < haveToSchedule->top; i++){
                pthread_mutex_init(&mutex[i], NULL);
                if ((rc1 = pthread_create(&threads[i], NULL, &sjfFunction, /*argumentos em forma de uma struct*/))){
                    printf("Thread creation failed: %d\n", rc1);
                    exit(EXIT_FAILURE);
                }
                pthread_mutex_unlock(mutex[/*identificador da trhead criada*/]);
            }
        }
    }
    return;
}


void *SJF(bob *toArrive, int optional)
{
    pthread_t threads;
    //pthread_t threads[sysconf(_SC_NPROCESSORS_ONLN)];
    //pthread_mutex_t mutex[sysconf(_SC_NPROCESSORS_ONLN)*2];
    //for (int i = 0; i < sysconf(_SC_NPROCESSORS_ONLN)*2; i++)
    //    pthread_mutex_init(&mutex[i], NULL);
    unsigned double time = 0.0;
    bob *haveToSchedule = creatBob();
    while (haveToSchedule->top == 0 && toArrive->top == 0){
        bob *new_process =  getNewProcess(toArrive, time);
        for (int i = 0; i < new_process->top; i++)
            addProcess(haveToSchedule, new_process->process_list[i]);
        if (haveToSchedule->top == 0){
            nanosleep(0, 100000000L);
            time += 0.1;
        }
        else{
            if ((rc1 = pthread_create(&threads[i], NULL, &sjfFunction, /*argumentos em forma de uma struct*/))){
                printf("Thread creation failed: %d\n", rc1);
                exit(EXIT_FAILURE);
            }
            pthread_join(threads, NULL);
        }
    }
    return;
}


int main(int argc, char **argv)
{
    pthread_t scheduler;
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *file_input = argv[2], *file_output = argv[3];
    if(argc >= 4 && !strcmp("d", argv[4]))
        optional = 1;
    if (scheduler_type == 1){
        bob *toArrive = getListOfProcesses(file_input);
        //SJF(toArrive, optional);
        if ((rc1 = pthread_create(&scheduler, NULL, &SJF, /*argumentos em forma de uma struct*/))){
            printf("Thread creation failed: %d\n", rc1);
            exit(EXIT_FAILURE);
        }
    }

    pthread_join(scheduler, NULL);
    return 0;
}
