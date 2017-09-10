// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include <string.h> // strcmp().
#include <stdlib.h> // atoi().
#include <time.h>  // nanosleep(), clock().
#include <unistd.h> // sleep().
#include <pthread.h> // pthread.
#include <math.h> // pow().
#include "fileHandler.h" // Implementa funções para lidar com input e output.

#define QUANTUM 0.5
#define NAP_TIME 0.5

pthread_mutex_t mutex; // Semáforo usado para parar os escalonadores.
double clock_time = 0; // Tempo que o programa usou para rodar.
int cs_counter = 0; // Número de mudanças de contexo.
typedef struct  {
                Process *procc;
                int optional;
                } thread_args;

// Recebe um double e irá consumir tempo real igual a esse número em segundos.
void nap(double dt)
{
    double temp;
    int temp2 = dt;
    temp = dt - temp2;
    temp *= 10;
    nanosleep((const struct timespec[]){{temp2, temp*100000000L}}, NULL);
    clock_time += dt; //atualização do tempo decorrido após o uso da função
    return;
}

/* Recebe um ponteiro para void que irá ser transformado no tipo List e simula
 * o processo apontado por ela p/ o escalonador SJF. */
void *simulateProcSJF(void *to_schedule)
{
    nap(((List)to_schedule)->info->dt);
    return NULL;
}

// Recebe uma lista ligada e irá devolver o node com o menor dt.
List getNextShortest(List root)
{
    List shortest = root;
    for (;root != NULL; root = root->next) {
        if (shortest->info->dt > root->info->dt)
            shortest = root;
    }
    return shortest;
}

/* Recebe o nome de dois arquivos, um de input e outro de output, junto com
 * um int optional que se for igual a 1 irá imprimir informações sobre os
 * processos. A função simula um escalonador de processos do tipo Shortest Job
 * First e imprime a saída dos processos no arquivo de output. */
void SJF(char *input_file, char *output_file, int optional)
{
    pthread_t thread;
    List to_arrive, shortest, to_schedule = NULL;
    to_arrive = readFile(input_file); // Lê o arquivo de entrada.

    /* O escalonador funciona até todos os processos tiverem sido escalonados
     * e não há mais nehum processo para chegar. */
    while ((to_schedule != NULL || to_arrive != NULL)) {
        clock_t begin = clock(); // Cálculo do tempo gasto pelo escalonador.

        // Verifica se novos processos novos chegaram no sistema.
        to_schedule = add(to_schedule, &to_arrive, clock_time, optional);
        if (to_schedule == NULL) {
            nap(NAP_TIME);
            continue;
        }

        // Procura qual processo deve ser escalonado (o menor).
        shortest = getNextShortest(to_schedule);
        if (optional == 1)
            fprintf(stderr, "O processo %s começou usar a CPU %d.\n",
                    shortest->info->name, 1);

        // Cálculo do tempo gasto pelo escalonador.
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;

        // Simula o processo.
        if (pthread_create(&thread, NULL, &simulateProcSJF, (void *)shortest)) {
            fprintf(stderr, "ERRO: Não foi possível criar thread.\n");
            exit(EXIT_FAILURE);
        }
        // Executa a thread até acabar e depois volta para o escalonador.
        if (pthread_join(thread, NULL)) { // Espécie de "semáforo".
            fprintf(stderr, "ERRO: Não foi possível dar join na thread.\n");
            exit(EXIT_FAILURE);
        }

        begin = clock(); // Cálculo do tempo gasto pelo escalonador.
        if (optional == 1)
            fprintf(stderr, "O processo %s deixou de usar a CPU %d.\n",
                    shortest->info->name, 1);

        // Escreve os tempos do processo que acabou no arquivo de saída.
        writeFile(output_file, shortest->info, clock_time, optional);
        // Como o processo acabou, então ele é removido da lista.
        to_schedule = removeList(to_schedule, shortest->info);
        // Cálculo do tempo gasto pelo escalonador.
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;
    }

    return;
}

/* Recebe um processo e simula ele por um QUANTUM ou pelo tempo restante que
 * ele possui. A função usa semáforos para simular a preempção. */
void *simulateProcRR(void *args)
{
    thread_args *t_args = emalloc(sizeof(thread_args));
    *t_args = *((thread_args *)args);

    while (t_args->procc->dt != t_args->procc->run_time) {
        pthread_mutex_lock(&(t_args->procc->mutex));
        if (t_args->procc->dt - t_args->procc->run_time > QUANTUM) {
            nap(QUANTUM);
            t_args->procc->run_time += QUANTUM;
            cs_counter++;
            if (t_args->optional)
                fprintf(stderr, "Mudança de contexto %d\n", cs_counter);
        }
        else {
            nap(t_args->procc->dt - t_args->procc->run_time);
            t_args->procc->run_time += t_args->procc->dt - t_args->procc->run_time;
        }
        pthread_mutex_unlock(&mutex);
    }
    free(t_args);
    return NULL;
}

/* Calcula a prioridade (número entre 1 e 10) de um dado processo, quanto mais
 * perto o tempo atual (clock_time) da deadline do processo mais prioridade o
 * processo ganha, se a deadline de um processo passar, depois de um certo
 * tempo a prioridade vira uma constante. */
int calcPriority(Process *proc)
{
    // A função é -((clock_time - deadline)/dt)^2 + 10.
    double priority = -pow(((clock_time - proc->deadline)/proc->dt), 2) + 10;
    if (priority < 1)
        return 1; // Limite inferior para a função de segundo grau.
    return (int)(priority + 1);
}

/* Recebe um processo e simula ele por um múltiplo de um QUANTUM (prioridade
 * do processo) ou pelo tempo restante que ele possui. A função usa semáforos
 * para simular a preempção. */
void *simulateProcPRR(void *args)
{
    int priority;
    thread_args *t_args = emalloc(sizeof(thread_args));
    *t_args = *((thread_args *)args);

    while (t_args->procc->run_time < t_args->procc->dt) {
        pthread_mutex_lock(&(t_args->procc->mutex));
        priority = calcPriority(t_args->procc);

        if (t_args->procc->dt - t_args->procc->run_time > QUANTUM*priority) {
            nap(QUANTUM*priority);
            t_args->procc->run_time += QUANTUM*priority;
            cs_counter++;
            if (t_args->optional)
                fprintf(stderr, "Mudança de contexto %d\n", cs_counter);
        }
        else {
            nap(t_args->procc->dt - t_args->procc->run_time);
            t_args->procc->run_time += t_args->procc->dt - t_args->procc->run_time;
        }
        pthread_mutex_unlock(&mutex);
    }
    free(t_args);
    return NULL;
}

/* Recebe o nome de dois arquivos, um de input e outro de output, junto com um
 * int optional que se for 1 irá imprimir informações sobre os processos e um
 * int schedular_type que se for 2 indica o escalonador Round Robin e se for
 * outro número indica escalonador com prioridade. A função simula um
 * escalonador de processos do tipo Round Robin ou Escalonador com prioridade
 * e imprime a saída dos processos no arquivo de output. */
void roundRobin(char *input_file, char *output_file, int optional,
                int scheduler_type)
{
    List to_arrive, to_schedule = NULL;
    List curr_process = NULL;
    to_arrive = readFile(input_file); // Lê o arquivo de entrada.
    thread_args *args = emalloc(sizeof(thread_args));
    pthread_mutex_init(&mutex, NULL);

    /* O escalonador funciona até todos os processos tiverem sido escalonados
     * e não há mais nehum processo para chegar. */
    pthread_mutex_lock(&mutex);
    while (to_schedule != NULL || to_arrive != NULL) {
        clock_t begin = clock(); // Cálculo do tempo gasto pelo escalonador.

        // Verifica se novos processos novos chegaram no sistema.
        to_schedule = add(to_schedule, &to_arrive, clock_time, optional);
        if (to_schedule == NULL) { // Os próximos processos não chegaram ainda.
            nap(NAP_TIME);
            continue;
        }

        // Faz a lista de processos ser "circular".
        if (curr_process == NULL)
           curr_process = to_schedule;

        // Cálculo do tempo gasto pelo escalonador.
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;

        // Cria uma nova thread para o processo se ela acabou de chegar.
        if (curr_process->info->run_time == 0) {
            pthread_mutex_init(&(curr_process->info->mutex), NULL);
            if (optional == 1)
                fprintf(stderr, "O processo %s começou usar a CPU %d.\n",
                        curr_process->info->name, 1);
            // Escolhe o tipo de escalonador.
            args->procc = curr_process->info;
            args->optional = optional;
            if (scheduler_type == 2) {
                if (pthread_create(&(curr_process->info->thread), NULL,
                                    &simulateProcRR, args)) {
                    fprintf(stderr, "ERRO: Não foi possível criar thread.\n");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                if (pthread_create(&(curr_process->info->thread), NULL,
                                    &simulateProcPRR, args)) {
                    fprintf(stderr, "ERRO: Não foi possível criar thread.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        // Chama a função que simula o processo.
        else {
            if (optional == 1)
                fprintf(stderr, "O processo %s começou usar a CPU %d\n",
                        curr_process->info->name, 1);
            pthread_mutex_unlock(&(curr_process->info->mutex));
        }

        // Volta para o escalonador.
        pthread_mutex_lock(&mutex);
        begin = clock(); // Cálculo do tempo gasto pelo escalonador.
        if (optional == 1) {
            fprintf(stderr, "O processo %s deixou de usar a CPU %d\n",
                    curr_process->info->name, 1);
        }

        /* Se o processo acabou, ele é inserido no output_file e retirado da
         * lista de processos e tem sua thread terminada. */
        if (curr_process->info->run_time >= curr_process->info->dt) {
            writeFile(output_file, curr_process->info, clock_time, optional);
            pthread_mutex_destroy(&(curr_process->info->mutex));
            pthread_cancel(curr_process->info->thread);
            if (pthread_join(curr_process->info->thread, NULL)) {
                fprintf(stderr, "ERRO: Não foi possível dar join na thread.\n");
                exit(EXIT_FAILURE);
            }
            List temp = curr_process->next;
            to_schedule = removeList(to_schedule, curr_process->info);
            curr_process = temp;
            continue;
        }
        curr_process = curr_process->next;
        // Cálculo do tempo gasto pelo escalonador.
        end = clock(); //calculo do tempo gasto pelo escalonador.
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;
    }
    free(args);

    return;
}

int main(int argc, char **argv)
{
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *input_file, *output_file;
    input_file = argv[2];
    output_file = argv[3];
    FILE *fp;
    // Verifica se o argumento opcional foi feito.
    if (argc >= 5 && !strcmp("d", argv[4]))
        optional = 1;
    // Verifica o tipo do escalonador e chama a função de acordo.
    if (scheduler_type == 1)
        SJF(input_file, output_file, optional);
    else
        roundRobin(input_file, output_file, optional, scheduler_type);
    // Escreve no arquivo de saída o total de mudanças de contexto do programa.
    fp = fopen(output_file, "a");
    fprintf(fp, "%d\n", cs_counter);
    fclose(fp);
    return 0;
}
