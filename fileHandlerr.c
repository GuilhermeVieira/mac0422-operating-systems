#include "fileHandlerr.h"

List readFile(char *fileName)
{
    List processQueue = createList();
    int n;
    FILE *file_pointer;
    char *line = NULL;
    char **parsedLine;
    size_t len = 0;
    ssize_t read;
    Process *temp;
    file_pointer = fopen(fileName, "r");

    if (file_pointer == NULL) {
        fprintf(stderr, "ERRO: Arquivo não encontrado.");
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, file_pointer)) != -1) {
        parsedLine = parseCommand(line, &n);
        if(parsedLine[0][0] == '\0')
            continue;
        temp = createProcess(atof(parsedLine[0]), atof(parsedLine[1]), atof(parsedLine[2]), parsedLine[3]);
        processQueue = addList(processQueue, temp);
    }
    fclose (file_pointer);
    return processQueue;
}

List createList()
{
    List root = NULL;
    return root;
}

List addList(List root, Process *new_process)
{
    if (root == NULL){
        root = emalloc(sizeof(Cell));
        root->info = new_process;
        root->next = NULL;
        return root;
    }
    root->next = addList(root->next, new_process);
    return root;
}

List removeList(List root, Process *x)
{
    if (root == NULL)
        return root;
    if (!strcmp(root->info->name, x->name)){
       List temp = root->next;
       destroyProcess(root->info);
       free(root);
       return temp;
    }
    root->next = removeList(root->next, x);
    return root;
}

Process *createProcess(double t0, double dt, double dl, char *name)
{
    Process *new_process = emalloc(sizeof(Process));
    new_process->t0 = t0;
    new_process->dt = dt;
    new_process->deadline = dl;
    new_process->priority = 0;
    new_process->name = name;
    return new_process;
}

void destroyProcess(Process *x)
{
    free(x->name);
    free(x);
    return;
}
