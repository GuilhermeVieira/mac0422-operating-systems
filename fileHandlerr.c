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
