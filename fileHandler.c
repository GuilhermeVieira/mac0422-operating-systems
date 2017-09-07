// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "fileHandler.h"

int output_line = 1;

int cmpfunc(const void *a, const void *b)
{
    if ((*(Process*) a).t0 < (*(Process*) b).t0) return -1;
    else if ((*(Process*) a).t0 ==  (*(Process*) b).t0) return 0;
    return 1;
}

void writeFile(char *output_file, Process *proc, double time, int optional){
    FILE *fp;
    fp = fopen(output_file, "a");
    fprintf(fp, "%s %.1f %.1f\n", proc->name, time, time - proc->t0);
    if (optional == 1)
        fprintf(stderr, "O processo %s foi finalizado e está sendo escrito na "
                        "linha %d.\n", proc->name, output_line);
    output_line++;
    fclose(fp);
    return;
}

List readFile(char *file_name)
{
    List process_queue = createList();
    int n, size = 1, i = 0, j = 0;
    FILE *file_pointer;
    char *line = NULL;
    char **parsed_line;
    size_t len = 0;
    ssize_t read;
    Process *temp;
    Process *array = emalloc(sizeof(Process));
    file_pointer = fopen(file_name, "r");

    // Verifica se o arquivo existe.
    if (file_pointer == NULL) {
        fprintf(stderr, "ERRO: Arquivo não encontrado.\n");
        exit(EXIT_FAILURE);
    }
    // Lê cada linha do arquivo de entrada e cria um processo para ela.
    while ((read = getline(&line, &len, file_pointer)) != -1) {
        parsed_line = parseCommand(line, &n);
        if(parsed_line[0][0] == '\0')
            continue;
        temp = createProcess(atof(parsed_line[0]), atof(parsed_line[1]),
                            atof(parsed_line[2]), parsed_line[3], i + 1);
        if (i >= size){
            size *= 2;
            array = erealloc(array, size*sizeof(Process));
        }
        array[i] = *temp;
        i++;
        for (int k; k < 3; k++)
            free(parsed_line[k]);
        free(temp);
        free(line);
        len = 0;
        free(parsed_line);
    }
    // Ordena os processos por t0.
    qsort(array, i, sizeof(Process), cmpfunc);
    // Adiciona os processos processosQueue.
    while (j < i) {
        process_queue = addList(process_queue, &array[j]);
        j++;
    }
    free(line);
    fclose (file_pointer);

    return process_queue;
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

Process *createProcess(double t0, double dt, double dl, char *name, int line)
{
    Process *new_process = emalloc(sizeof(Process));
    new_process->t0 = t0;
    new_process->dt = dt;
    new_process->deadline = dl;
    new_process->line = line;
    new_process->run_time = 0.0;
    new_process->name = name;
    return new_process;
}

void destroyProcess(Process *x)
{
    free(x->name);
    return;
}

List add(List to_schedule, List *to_arrive, double time, int optional)
{
    List *temp, *head, tail;
    head = emalloc(sizeof(List));
    temp = emalloc(sizeof(List));
    *head = *to_arrive;

    // Verifica se processos novos chegaram.
    for (; *to_arrive != NULL && (*to_arrive)->info->t0 <= time;
        *temp = *to_arrive, *to_arrive = (*to_arrive)->next)
        if (optional == 1)
            fprintf(stderr, "O processo %s presente na linha %d do arquivo de "
                            "trace chegou ao sistema.\n",
                            (*to_arrive)->info->name, (*to_arrive)->info->line);
    // Adiciona processos que chegaram na to_schedule.
    if (*head != *to_arrive && (*head)->info->t0 <= time){
        tail = getTail(to_schedule);
        if (tail == NULL)
            to_schedule = *head;
        else
            tail->next = *head;
        (*temp)->next = NULL;
    }
    free(head);
    free(temp);
    return to_schedule;
}

List getTail(List head)
{
    List tail = head;
    if (head == NULL) return head;
    while (tail->next != NULL)
        tail = tail->next;
    return tail;
}
