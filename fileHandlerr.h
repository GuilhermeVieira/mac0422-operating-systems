#ifndef __FILEHANDLERR__
#define __FILEHANDLERR__

#define MAXNUMBER 8;
#define QUANTUM 1;

typedef struct {
                int priority;
                double t0, dt, deadline;
                char *name;
                } process;

typedef struct  {
                process *process_list;
                int max;
                int top;
                } bob;

process *createProcess(double t0, double dt, double deadline, char *name);

void destroyProcess(process *P);

bob *creatBob();

void destroyBob();

void addProcess(bob *Bob, bob *new_process);


bob *getListOfProcesses(char *file_input);


#endif
