#ifndef __FILEHANDLERR__
#define __FILEHANDLERR__

#define MAXNUMBER 8;
#define QUANTUM 1;

typedef struct {
                int priority;
                double t0, dt, deadline;
                char *name;
                } process;


process *createProcess(double t0, double dt, double deadline, char *name);

void destroyProcess(process *P);


#endif
