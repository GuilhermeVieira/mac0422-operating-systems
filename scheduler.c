#include <string.h> //strcmp()
#include <stdlib.h> //atoi()
#include "fileHandlerr.h"

int main(int argc, char **argv)
{
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *inputFile, *outputFile;
    List toArrive, toSchedule = NULL;
    inputFile = argv[2];
    outputFile = argv[3];
    if (argc >= 4 && strcmp("d", argv[4])){
        optional = 1;
    }
    toArrive = readFile(inputFile);
    double time = 0;
    while((toSchedule != NULL || toArrive != NULL)){
        toSchedule = add(toSchedule, &toArrive, time);
        printf("%f\n", time);
        time++;
        for(List temp = toSchedule ;temp != NULL; temp = temp->next)
            printf("%s\n", temp->info->name);
        printf("/////////\n");
        for(List temp = toArrive ;temp != NULL; temp = temp->next)
            printf("%s\n", temp->info->name);
    }
    return 0;
}
//toArrive vira NULL dps do add
