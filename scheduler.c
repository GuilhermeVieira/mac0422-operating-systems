#include <stdio.h>
#include <stdlib.h> //atoi
#include <string.h> //strcmp
#include <pthread.h> //threads
#include <time.h> //nanosleep
#include <unistd.h> //sysconf

void SJF(bob *toArrive, int optional)
{
    pthread_mutex_t mutex[sysconf(_SC_NPROCESSORS_ONLN)];
    for (int i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
        pthread_mutex_init(&mutex[i], NULL);
    unsigned double time = 0.0;
    bob *haveToSchedule = creatBob();
    while (haveToSchedule->top == 0 && toArrive->top == 0){
        bob *new_process =  getNewProcess(toArrive, time);
        for (int i = 0; i < new_process; i++)
            addProcess(haveToSchedule, new_process->process_list);
        if (haveToSchedule->top == 0)
            nanosleep(0, 500000000L);
        else {
            //cria uma thread na thred tem um semáforo para o escalonador parar
        }
    }
    return;
}


int main(int argc, char **argv)
{
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *file_input = argv[2], *file_output = argv[3];
    if(argc >= 4 && !strcmp("d", argv[4]))
        optional = 1;
    if (scheduler_type == 1){
        bob *toArrive = getListOfProcesses(file_input);
        SJF(toArrive, optional);
    }
    return 0;
}
