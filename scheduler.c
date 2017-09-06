// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include <string.h> //strcmp().
#include <stdlib.h> //atoi().
#include <time.h>  //nanosleep(), clock().
#include <unistd.h> //sleep().
#include <pthread.h> //pthreads.
#include <math.h> //pow().
#include "fileHandler.h"

#define QUANTUM 0.5
#define NAP_TIME 0.5

pthread_mutex_t mutex; //semáforo usado para parar os escalonadores.
double clock_time = 0; //tempo que o programa usou para rodar.
int cs_counter = 0; //número de mudanças de contexo.

/*Função recebe um double e ira consumir tempo igual a esse número em segundos.
 */
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

/*Função recebe um ponteriro para void que ira ser transformado no tipo List e
 *ira simular o processo apontado por ela.
 */
void *simulateProcSJF(void *toSchedule)
{
    nap(((List)toSchedule)->info->dt);
    return NULL;
}

/*Função recebe uma parte de uma lista ligada e ira devolver o node com o menor dt.
 */
List getNextShortest(List root)
{
    List shortest = root;
    for (;root != NULL; root = root->next){
        if (shortest->info->dt > root->info->dt)
            shortest = root;
    }
    return shortest;
}
/*Função recebe o nome de dois arquivos, um de input e outro de output, junto
 *com um int que se for 1 ira imprimir informações sobre os processos. A função
 *simula um escalonador de processos do tipo Shortest Job First e imprime a
 *saida dos processos no arquivo de output.
 */
void SJF(char *inputFile, char *outputFile, int optional)
{
    pthread_t thread;
    List toArrive, shortest, toSchedule = NULL;
    toArrive = readFile(inputFile);
    /*O escalonador deve funcionar até todos os processos tiverem sido escalonados
     *e não há mais nehum processo para chegar.
     */
    while((toSchedule != NULL || toArrive != NULL)){
        clock_t begin = clock(); //calculo do tempo gasto pelo escalonador.
        toSchedule = add(toSchedule, &toArrive, clock_time, optional); //ve se processos novos chegaram ao sistema.
        if (toSchedule == NULL){
            nap(NAP_TIME);
            continue;
        }
        shortest = getNextShortest(toSchedule); //procura qual processo devera ser escalonado.
        if (optional == 1)
            fprintf(stderr, "O processo %s começou usar a CPU %d\n", shortest->info->name, 1);
        clock_t end = clock(); //calculo do tempo gasto pelo escalonador.
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;
        pthread_create(&thread, NULL, &simulateProcSJF, (void *)shortest); //simula o processo.
        pthread_join(thread, NULL); //funciona como um semáforo para a função que chamou uma thread.
        begin = clock(); //calculo do tempo gasto pelo escalonador.
        if (optional == 1)
            fprintf(stderr, "O processo %s deixou de usar a CPU %d\n", shortest->info->name, 1);
        cs_counter++; //adiciona no número de mudançãs de contexto.
        if (optional == 1)
            fprintf(stderr, "Mudança de contexto %d\n", cs_counter);
        writeFile(outputFile, shortest->info, clock_time, optional);
        toSchedule = removeList(toSchedule, shortest->info); //como o processo acabou ele é removido da lista.
        end = clock(); //calculo do tempo gasto pelo escalonador.
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;
    }
    return;
}

/*Função recebe um processo e ira simular ela por um QUANTUM ou pelo tempo
 *restante que ela possui se esse for menor que um QUANTUM, a função usa
 *semáforos para simular a mudança de contexto.
 */
void *simulateProcRR(void *proc){
    Process *procc = (Process *)proc;
    while (procc->dt != procc->run_time){
        pthread_mutex_lock(&(procc->mutex));
        printf("estou rodando o processo %s\n", procc->name);
        if (procc->dt - procc->run_time >= QUANTUM){
            nap(QUANTUM);
            procc->run_time += QUANTUM;
        }
        else{
            nap(procc->dt - procc->run_time);
            procc->run_time += procc->dt - procc->run_time;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/*Função calcula a prioridade (número entre 1 e 10) de um dado processo, uma
 *equação de segundo grau é usada devido sua simetria, quanto mais perto o tempo
 *atual (clock_time) da deadline do processo mais prioridade o processo ganha,
 *se a deadline de um processo passar depois de um certo tempo a prioriade vira
 *uma constante.
 */
int calcPriority(Process *proc)
{
    //a função é -((clock_time - deadline)/dt)^2 + 10.
    double priority = -pow(((clock_time - proc->deadline)/proc->dt), 2) + 10;
    if (priority > 1) {
    	if ((int)(priority + 1) < 5 && clock_time > proc->deadline) return 5;
    	return (int)(priority + 1);
    }
    return 1; //defini um limite inferior para a função de segundo grau.
}

/*Função recebe um processo e ira simular ela por um múltiplo de um QUANTUM ou
 *pelo tempo restante que ela possui se esse for menor que esse tempo, a função usa
 *semáforos para simular a mudança de contexto.
 */
void *simulateProcPRR(void *proc){
	int priority;
    Process *procc = (Process *)proc;
    while (procc->dt != procc->run_time){
        pthread_mutex_lock(&(procc->mutex));
        priority = calcPriority(procc);
        if (procc->dt - procc->run_time >= QUANTUM*priority){
            nap(QUANTUM*priority);
            procc->run_time += QUANTUM*priority;
        }
        else{
            nap(procc->dt - procc->run_time);
            procc->run_time += procc->dt - procc->run_time;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/*Função recebe o nome de dois arquivos, um de input e outro de output, junto
 *com um int que se for 1 ira imprimir informações sobre os processos e um int
 *que se 2 indica roundRobin se outro número indica Escalonador com prioridade.
 *A função simula um escalonador de processos do tipo Round Robin ou Escalonador
 *com prioridade e imprime a saida dos processos no arquivo de output.
 */
void roundRobin(char *inputFile, char *outputFile, int optional, int scheduler_type)
{
    List toArrive, toSchedule = NULL;
    List currProcess = NULL;
    toArrive = readFile(inputFile);
    pthread_mutex_init(&mutex, NULL);
    /*O escalonador deve funcionar até todos os processos tiverem sido escalonados
     *e não há mais nehum processo para chegar.
     */
    while(toSchedule != NULL || toArrive != NULL){
        clock_t begin = clock(); //calculo do tempo gasto pelo escalonador.
        toSchedule = add(toSchedule, &toArrive, clock_time, optional); //ve se processos novos chegaram ao sistema.
        if (toSchedule == NULL){
            nap(NAP_TIME);
            continue;
        }
        if (currProcess == NULL)
           currProcess = toSchedule;
        clock_t end = clock(); //calculo do tempo gasto pelo escalonador.
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;
        if (currProcess->info->run_time == 0){
            pthread_mutex_init(&(currProcess->info->mutex), NULL);
            if (optional == 1)
                fprintf(stderr, "O processo %s começou usar a CPU %d\n", currProcess->info->name, 1);
            if (scheduler_type == 2)
                pthread_create(&(currProcess->info->thread), NULL, &simulateProcRR, currProcess->info);
            else
                pthread_create(&(currProcess->info->thread), NULL, &simulateProcPRR, currProcess->info);
        }
        else{
            if (optional == 1)
                fprintf(stderr, "O processo %s começou usar a CPU %d\n", currProcess->info->name, 1);
            pthread_mutex_unlock(&(currProcess->info->mutex));
        }
        pthread_mutex_lock(&mutex);
        begin = clock(); //calculo do tempo gasto pelo escalonador.
        cs_counter++;
        if (optional == 1){
            fprintf(stderr, "O processo %s deixou de usar a CPU %d\n", currProcess->info->name, 1);
            fprintf(stderr, "Mudança de contexto %d\n", cs_counter);
        }
        /* Se o processo acabou ele deve ser inserido no outputFile, ser retirado
         *da lista de processos e ter sua thread terminada.
         */
        if (currProcess->info->run_time >= currProcess->info->dt){
            writeFile(outputFile, currProcess->info, clock_time, optional);
            pthread_mutex_destroy(&(currProcess->info->mutex));
            pthread_cancel(currProcess->info->thread);
            List temp = currProcess->next;
            toSchedule = removeList(toSchedule, currProcess->info);
            currProcess = temp;
            continue;
        }
        currProcess = currProcess->next;
        end = clock(); //calculo do tempo gasto pelo escalonador.
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        clock_time += time_spent;
    }
    return;
}

int main(int argc, char **argv)
{
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *inputFile, *outputFile;
    inputFile = argv[2];
    outputFile = argv[3];
    FILE *fp;
    if (argc >= 5 && !strcmp("d", argv[4]))
        optional = 1;
    if (scheduler_type == 1)
        SJF(inputFile, outputFile, optional);
    else
        roundRobin(inputFile, outputFile, optional, scheduler_type);
    fp = fopen(outputFile, "a");
    fprintf(fp, "%d\n", cs_counter);
    fclose(fp);
    return 0;
}
