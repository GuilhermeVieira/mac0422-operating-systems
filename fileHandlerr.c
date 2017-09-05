#include "fileHandlerr.h"

int cmpfunc(const void *a, const void *b)
{
    if ((*(Process*) a).t0 < (*(Process*) b).t0) return -1;
    else if ((*(Process*) a).t0 ==  (*(Process*) b).t0) return 0;
    return 1;
}

void writeFile(char *outputFile, Process *proc, double time){
    FILE *fp;
    fp = fopen(outputFile, "a");
    fprintf(fp, "%s %f %f\n", proc->name, time, time - proc->t0);
    if (optional == 1)    
        fprintf(stderr, "O processo %s foi finalizado e esta sendo escrito na linha %d", proc->name, /*número da linha*/); //mudei, falta achar a linha    

    fclose(fp);
    return;
}

List readFile(char *fileName)
{
    List processQueue = createList();
    int n, size = 1, i= 0, j = 0;
    FILE *file_pointer;
    char *line = NULL;
    char **parsedLine;
    size_t len = 0;
    ssize_t read;
    Process *temp;
    Process *array = emalloc(sizeof(Process));
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
        if (i >= size){
            size *= 2;
            array = erealloc(array, size*sizeof(Process));
        }
        array[i] = *temp;
        i++;
    }
    qsort(array, i, sizeof(Process), cmpfunc);
    while (j < i) {
        processQueue = addList(processQueue, &array[j]);
        j++;
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
       destroyProcess(root->info); //bugada;
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
    new_process->run_time = 0.0;
    new_process->name = name;
    return new_process;
}

void destroyProcess(Process *x)
{
    free(x->name);
    //free(x); isso que esta bugado
    return;
}

List add(List toSchedule, List *toArrive, double time)
{
    List *temp, *head, tail;
    head = emalloc(sizeof(List));
    temp = emalloc(sizeof(List));
    *head = *toArrive;
    for (; *toArrive != NULL && (*toArrive)->info->t0 <= time; *temp = *toArrive, *toArrive = (*toArrive)->next)
        if (optional == 1)                                                        //adicionei esse if
            fprintf(stderr, "o processo %s presente na linha %d do arquivo de trace chegou ao sistema", (*toArrive)->info->name, /*alguma coisa com a linha de arquivo*/);
    
    if (*head != *toArrive && (*head)->info->t0 <= time){
        tail = getTail(toSchedule);
        if (tail == NULL)
            toSchedule = *head;
        else
            tail->next = *head;
        (*temp)->next = NULL;
    }
    return toSchedule;
}

List getTail(List head)
{
    List tail = head;
    if (head == NULL) return head;
    while (tail->next != NULL)
        tail = tail->next;
    return tail;
}
