#include "fileHandlerr.h"
#include "buffer.h"


process *createProcess(double t0, double dt, double deadline, char *name)
{
    process *new_process = emalloc(sizeof(process));
    new_process->priority = 0;
    new_process->t0 = t0;
    new_process->dt = dt;
    new_process->deadline= deadline;
    new_process->name = name;
    return new_process;
}

void destroyProcess(process *P)
{
    free(P->name);
    free(P);
    return;
}

bob *creatBob()
{
    bob *newBob = emalloc(sizeof(bob));
    newBob->max = MAXNUMBER;
    newBob->top = 0;
    return newBob;
}

void destroyBob()
{
    free(bob->process_list);
    free(bob);
    return;
}

void addProcess(bob *Bob, process *new_process)
{
    int i = 0;
    if (Bob->max <= Bob->top){
        Bob->max *= 2;
        Bob->process_list = erealloc(Bob->process_list, Bob->max)
    }
    i = binSearch();
    for (int k = Bob->top; k >= i; k--)
        Bob->process_list[k + 1] = Bob->process_list[k];
    Bob->process_list[i] = new_process;
    destroyProcess(new_process);
    Bob->top++;
    return;
}

bob *getListOfProcesses(char *file_input)
{
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char **parsed_line;
    bob *BOB = creatBob();
    process *temp ;
    file = fopen(file_input, "r");
    if (file == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1){
        parsed_line = parseCommand(line, &len);
        temp = createProcess(atof(parsed_line[0]), atof(parsed_line[1]), atof(parsed_line[2]), parsed_line[3]);
        if (BOB->max <= BOB->top){
            BOB->max *= 2;
            BOB->process_list = erealloc(BOB->process_list, BOB->max)
        }
        BOB->process_list[BOB->top]  = temp;
        BOB->top++;
        destroyProcess(temp);
    }
    return BOB;
}

void *sjfFunction(, double dt, unsigned double *time)
{
    //pthread_mutex_lock(//& o mutex desse processo);
    nanosleep( (int) dt, (dt - dt/1)*100000000L);
    *time += dt;
    //pthread_mutex_unlock(//& o mutex do SJF);
    return;
}
