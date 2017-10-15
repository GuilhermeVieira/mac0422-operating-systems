// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include <stdlib.h> 
#include <time.h> 
#include <pthread.h> 
#include "globals.h"
#include "sort.h"
#include "track.h"
#include "linked_list.h"

typedef struct { uint d, n, v, tag, lucky; } thread_arg; // Arg dos ciclistas.
typedef struct { uint d, n, debug; } thread_report_arg; // Arg da thread report.

pthread_mutex_t n_cyclists_mutex; 
pthread_mutex_t check_points;
pthread_mutex_t sem_output;

pthread_barrier_t barrier1;
pthread_barrier_t barrier2;
pthread_t *thread_dummy; 

int refresh = MS60; // Taxa de atualização do começo da prova.
List to_print; // Lista para imprimir a pontuação acumulada da prova.

/* Procura elemento x em um vetor temp de tamanho n. Retorna 1 se x se
 * encontra em temp, 0 caso não se encontre. */
int inArray (rank *temp, uint x, uint n)
{
    uint i = 0;
    for (; i < n && temp[i].laps != x; i++) {}
    if (i != n && temp[i].laps == x) {
        return 1;
    }
    return 0;
}

/* Dado o número de voltas laps, retorna se o ciclista quebrou ou não ao
 * completar a volta. */
int break_cyclists(uint laps)
{
    int prob = rand()%100;
    if (laps%15 == 0 && prob <= 1) {
        pthread_mutex_lock(&n_cyclists_mutex);
        if (n_cyclists <= 5) {
            pthread_mutex_unlock(&n_cyclists_mutex);
            return 0;
        }
        pthread_mutex_unlock(&n_cyclists_mutex);
        return 1;
    }
    return 0;
}

/* Dado uma velocidade em Km/h a função retorna a velocidade em (m/ms)*dt, onde 
 * dt está relacionado com o refresh rate atual. */
double velInRefreshTime (int velocity, int refresh)
{
    /*   Km   1000m   1 hora    1s
    ---- *----- * ------ * -----
    hora   1 Km   3600s    1000 ms
    */
    double vel = (double) velocity;
    if (refresh == 2)
        return (vel/3600)*60;
    else
        return (vel/3600)*20;
}

/* Retorna a nova velocidade de um ciclista dado sua velocidade antiga. */
int getNewVelocity (int velocity)
{
    int prob;

    prob = rand()%100;

    if (velocity == 30)
        if (prob <= 30)
            return velocity;
        else
            return 60;
    else if (velocity == 60)
        if (prob <= 50)
            return 30;
        else
            return velocity;
    else
        return velocity;
}

/* Dado uma pista global, seu tamanho, e as coordenadas do ciclista que irá se
 * mover (dado por pos). A função calcula sua nova posição, chama a updateTrack() 
 * e muda o argumento pos de acordo. Retorna 0 se foi póssivel mover o ciclista 
 * e 1 se ele estava impedido de fazer qualquer movimento. É garantido que ao 
 * entrar nessa função, o ciclista pode avançar uma posição na corrida. */
int updatePosition(position *pos, int length)
{
    position *old_pos = emalloc(sizeof(position));
    // Guarda posições antigas.
    old_pos->x = pos->x; 
    old_pos->y = pos->y;
    // Inicializa nova posição.
    int new_pos_x = pos->x - 1; // O ciclista deve avançar na matriz.
    int new_pos_y = pos->y;
    int ret = 0;
    // Ajusta a circularidade da pista. 
    if (new_pos_x < 0)
        new_pos_x = length - 1;
    // Guardam a posição logo a frente na matriz.
    int temp = new_pos_x; 
    int temp2 = pos->y;
    
    // Verifica se o cara imediatamente a frente está voltas atrás. 
    if (pista[new_pos_x][pos->y].pos > 0 && 
        ranking[pista[new_pos_x][pos->y].pos].laps < ranking[pista[old_pos->x][old_pos->y].pos].laps) {
        
        // Tenta passar o retardatalho pela pista mais externa.
        for (int j = pos->y + 1; j < LANES; j++) {
            if (pista[pos->x][j].pos == 0) {
                if (pista[new_pos_x][j].pos > 0) {
                    if (ranking[pista[new_pos_x][j].pos].laps == ranking[pista[old_pos->x][old_pos->y].pos].laps) {
                        pthread_mutex_lock(&pista[new_pos_x][j].mutex);
                        if (pista[new_pos_x][j].pos == 0) {
                            new_pos_y = j;
                            break; 
                        }
                        pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
                    }
                }
                else {
                    pthread_mutex_lock(&pista[new_pos_x][j].mutex);
                    if (pista[new_pos_x][j].pos == 0) {
                        new_pos_y = j;
                        break; 
                    }
                    pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
                }
            }
            else 
                break;
        }
        if (new_pos_y != old_pos->y) {
            // Conseguiu uma posição para passar o retardatalho.
            pos->x = new_pos_x;
            pos->y = new_pos_y;
            updateTrack(pos, old_pos);
            pthread_mutex_unlock(&pista[new_pos_x][new_pos_y].mutex);
        }
        else {
            // Não conseguiu passar o retardatalho. Reseta new_pos_x.
            new_pos_x = pos->x;
        }           
    }
    else {
        // Espera para acessar a posição imediatamente a frente.
        pthread_mutex_lock(&pista[new_pos_x][pos->y].mutex);
        
        // Verifica se existe um ciclista na posição imediatamente há frente.
        if (pista[new_pos_x][pos->y].pos == 0) {
            /* A posição imediatamente a frente está vazia. Então tento achar 
             * uma posição vazia mais interna (diagonal) */
            for (int j = pos->y -1; j >= 0; j--) {
                if (pista[pos->x][j].pos == 0) {
                    if (pista[new_pos_x][j].pos > 0) {
                        if (ranking[pista[new_pos_x][j].pos].laps == ranking[pista[old_pos->x][old_pos->y].pos].laps) {
                            pthread_mutex_lock(&pista[new_pos_x][j].mutex);
                            if (pista[new_pos_x][j].pos == 0) { 
                                new_pos_y = j;
                                break; 
                            }
                            pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
                        }
                    }
                    else { 
                        pthread_mutex_lock(&pista[new_pos_x][j].mutex);
                        if (pista[new_pos_x][j].pos == 0) {
                            new_pos_y = j;
                            break; 
                        }
                        pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
                    }
                }
                else
                    break;
            }
            pos->x = new_pos_x;
            pos->y = new_pos_y;

            if (old_pos->y != new_pos_y) {
                // Conseguiu uma posição mais interna.
                updateTrack(pos, old_pos); // Atualiza a posição na pista.
                pthread_mutex_unlock(&pista[temp][new_pos_y].mutex);
            }
            else {
                // Não conseguiu posição mais interna e portanto só avança.
                updateTrack(pos, old_pos);
            }
            pthread_mutex_unlock(&pista[new_pos_x][temp2].mutex);
        }
        else {
            /* A posição imediatamente a frente está ocupada. Então o ciclista
             * tenta ultrapassar pela pista mais externa. */ 
            for (int j = pos->y + 1; j < LANES; j++) {
                if (pista[pos->x][j].pos == 0) {
                    if (pista[new_pos_x][j].pos > 0) {
                        if (ranking[pista[new_pos_x][j].pos].laps == ranking[pista[old_pos->x][old_pos->y].pos].laps) {
                            pthread_mutex_lock(&pista[new_pos_x][j].mutex);
                            if (pista[new_pos_x][j].pos == 0) {
                                new_pos_y = j;
                                break; 
                            }
                            pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
                        }
                    }
                    else {
                        pthread_mutex_lock(&pista[new_pos_x][j].mutex);
                        if (pista[new_pos_x][j].pos == 0) {
                            new_pos_y = j;
                            break; 
                        }
                        pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
                    }
                }
                else 
                    break;
            }

            if (new_pos_y != old_pos->y) {
                // Conseguiu ultrapassar pela pista externa.
                pos->x = new_pos_x;
                pos->y = new_pos_y;
                updateTrack(pos, old_pos);
                pthread_mutex_unlock(&pista[new_pos_x][new_pos_y].mutex);
            }
            else {
                // Não conseguiu ultrapassar. Vai retornar blocked = 1.
                new_pos_x = pos->x;
            }   
            pthread_mutex_unlock(&pista[temp][temp2].mutex);
        }
    }

    pthread_mutex_unlock(&pista[old_pos->x][old_pos->y].mutex); //Unlock na pos antiga

    // Verifica se o ciclista se moveu ou foi bloqueado.
    if (new_pos_x == old_pos->x && new_pos_y == old_pos->y) ret = 1;
    free(old_pos);

    return ret;
}

/* Thread que imprime a classificação por pontos parcial e a ordem em que os 
 * ciclistas terminaram uma volta e verifica se algum ciclista conseguiu 
 * ultrapassar todos os outros mais de uma vez para atribuir uma bonificação 
 * nos pontos. */
void *report(void *args)
{
    thread_report_arg *arg = (thread_report_arg *) args;
    rank *temp = emalloc(arg->n*sizeof(rank));
    int first, last;
    uint first_place = 0;
    int laps_to_points = 1;
    uint last_lap = 0;

    while (1) {
        pthread_barrier_wait(&barrier1);
        // Inicializa o ranking temporário. 
        for (int i = 0; i < arg->n; i++) {
            temp[i].laps = ranking[i].laps;
            temp[i].pos = ranking[i].pos;
            temp[i].pts = ranking[i].pts;
            temp[i].tag = ranking[i].tag;
            temp[i].time_elapsed = ranking[i].time_elapsed;
            temp[i].broken_lap = ranking[i].broken_lap;
        }

        qsort(temp, arg->n, sizeof(rank), cmpLaps); // Ordena por voltas.

        first = last = 0;
        for (int i = 1; i < arg->n; ++i) {
            if (temp[i].laps == temp[i - 1].laps)
                last = i;
            else {
                sort_range_array(temp, first, last);
                first = i; last = i;
            }
        }
        sort_range_array(temp, first, last);
        if (first_place != temp[0].tag) {
            first_place = temp[0].tag;
            laps_to_points = 1;
        }
        if (temp[0].laps > temp[1].laps + laps_to_points && temp[0].pos.x < temp[1].pos.x) {
            ranking[0].pts += 20;
            laps_to_points++;
        }

        if (!inArray(temp, last_lap, arg->n)){
            pthread_mutex_lock(&sem_output);
            // Chama a função que imprime a ordem dos ciclistas que chegaram numa volta.
            to_print = printLap(to_print);
            // Imprime a classificação dos sprints. 
            if (last_lap%10 == 0 && last_lap != 0){
                printf("Pontuação após o ultimo ciclista ter completado a volta %u:\n", last_lap);
                qsort(temp, arg->n, sizeof(rank), cmpPts);
                for (int i = 0; i < arg->n; i++) {
                    if (temp[i].time_elapsed != -1)
                        printf("[tag = %u; pts = %u]\n", temp[i].tag, temp[i].pts);
                }

                for (int i = 0; i < arg->n; i++) {
                    if (temp[i].time_elapsed == -1)
                        printf("[tag = %u; QUEBRADO]\n", temp[i].tag);
                }

                printf("\n\n");
            }
            last_lap++;
            pthread_mutex_unlock(&sem_output);
        }
        // Imprime a matriz caso o debug esteja ativado.
        if (arg->debug && n_cyclists > 0) {
            pthread_mutex_lock(&sem_output);
            for (int i = 0; i < arg->d; i++) {
                for (int j = 0; j < LANES; j++) {
                    if (pista[i][j].pos == 0)
                        printf(" -  ");
                    else
                        printf("%3d ", pista[i][j].pos);
                }
                printf("\n");
            }
            printf("\n");
            pthread_mutex_unlock(&sem_output);
        }
        // Verifica se a corrida acabou.
        if (is_over) break;
        pthread_barrier_wait(&barrier2);
    }

    free(temp);
    pthread_exit(NULL);
    return NULL;
}

/* Thread dummy para completar a barreira quando os ciclistas terminam a prova
 * ou quebram. */
void *dummy()
{
    while (1) {
        pthread_barrier_wait(&barrier1);
        if (is_over) break;
        pthread_barrier_wait(&barrier2);
    }
    pthread_exit(NULL);
    return NULL;
}

/* Thread que simula um ciclista na corrida. */
void *ciclista(void *args)
{
    thread_arg *arg = (thread_arg *) args;
    int broken = 0;
    int velocity = 30;
    int updatePos = 0;
    int blocked = 0; // Vale 1 se o ciclista a frente está bloqueando a passagem.
    uint laps = 0;
    position *pos = emalloc(sizeof(position));
    pos->x = (arg->tag - 1)/10;
    pos->y = (arg->tag - 1)%10;
    ranking[arg->tag - 1].tag = arg->tag;
    ranking[arg->tag - 1].pos = *pos;
    ranking[arg->tag - 1].pts = 0;
    ranking[arg->tag - 1].time_elapsed = 0;
    ranking[arg->tag - 1].broken_lap = 0;
    ranking[arg->tag - 1].laps = laps;
    
    // Loop que simula a corrida. 
    while (laps <= arg->v && !broken){
        // Conta o tempo real que se passou desde o início da corrida.
        if (refresh == MS60)
            ranking[arg->tag - 1].time_elapsed += 0.06;
        else
            ranking[arg->tag - 1].time_elapsed += 0.02;
 
        if (!blocked) {
            // Acrescenta a velocidade no updatePos.
            updatePos = (updatePos + ((int) (velInRefreshTime(velocity, refresh)*refresh)))%refresh;
        }
        else {
            // Se estava bloqueado, o ciclista poderá avanaçar.
            updatePos = 0;
        }

        // Quando updatePos é zero, o ciclista pode avançar.
        if (updatePos == 0) {
            // Atualiza a posição do ciclista. 
            blocked = updatePosition(pos, arg->d);
            // Completou uma volta. 
            if (pos->x == arg->d - 1) {
                // Calcula nova velocidade se não está na primeira volta.
                if (laps != 0)
                    velocity = getNewVelocity(velocity); 
                laps++; 

                // Verifica se o ciclista ganhou pontos nessa volta e os atribui.
                pthread_mutex_lock(&check_points);
                for (int k = 0; k < 4; k++){
                    if (points_buffer[k] == laps){
                        points_buffer[k] += 10;
                        if (k == 0){
                            ranking[arg->tag - 1].pts += 5;
                            break;
                        }
                        else {
                            ranking[arg->tag - 1].pts += (4 - k);
                            break;
                        }
                    }
                }
                to_print = addList(to_print, laps, arg->tag);
                pthread_mutex_unlock(&check_points);
                
                // Verifica se o ciclista quebrou.
                broken = break_cyclists(laps);
                if (broken) {
                    ranking[arg->tag -1].time_elapsed = -1;
                    ranking[arg->tag - 1].broken_lap = laps;
                    pthread_mutex_lock(&check_points);
                    ranking[arg->tag - 1].pts = 0;
                    pthread_mutex_unlock(&check_points);
                    laps = 0;
                }
            }
        }
        else {
            /* Não se moveu, então libera a posição para outro ciclista ver
             * que ainda está na mesma posição. */ 
            pthread_mutex_unlock(&pista[pos->x][pos->y].mutex);
        }
        // Atualiza o ranking. 
        ranking[arg->tag -1].laps = laps;
        ranking[arg->tag -1].pos = *pos;

        pthread_barrier_wait(&barrier1); // Espera todo mundo calcular sua posição.

        pthread_mutex_lock(&pista[pos->x][pos->y].mutex);

        // Verifica se é o ciclista sortudo que vai andar a 90 km/h.
        if (arg->lucky == 1 && (arg->v - laps) == 2) {
            velocity = 90;
            refresh = MS60;
        }

        // Se o ciclista quebrou, imprime as informações necessárias.
        if (broken) {
            pthread_mutex_unlock(&pista[pos->x][pos->y].mutex);
            rank *temp = emalloc(arg->n*sizeof(rank));
            for (int i = 0; i < arg->n; i++)
                temp[i] = ranking[i];
            qsort(temp, arg->n, sizeof(rank), cmpPts);
            for (int i = 0; i < arg->n; i++) {
                if (temp[i].tag == arg->tag) {
                    pthread_mutex_lock(&sem_output);
                    printf("O corredor %u quebrou na volta %u e ele estava na posição %d\n\n", 
                            arg->tag, ranking[arg->tag -1].broken_lap, i+1);
                    pthread_mutex_unlock(&sem_output);
                    break;
                }
            }
            pista[pos->x][pos->y].pos = 0;
            free(temp);
        }
        if (laps > arg->v && !broken){
            pthread_mutex_unlock(&pista[pos->x][pos->y].mutex);
            pista[pos->x][pos->y].pos = 0;
        }
        pthread_barrier_wait(&barrier2); // Espera as impressões acontecerem.
    }

    /* Tira o ciclista da pista. */

    /* Atualiza o número de ciclistas na pista. */
    pthread_mutex_lock(&n_cyclists_mutex);
    n_cyclists--;
    pthread_mutex_unlock(&n_cyclists_mutex);

    /* Verifica se a corrida acabou e manda um sinal para todos pararem. */
    if (n_cyclists < 1) is_over = 1;

    /* Cria uma thread dummy para deixar a barreira funcional. */
    pthread_create(&(thread_dummy[arg->tag - 1]), NULL, &dummy, NULL);
    if (!broken)
        /* Espera a thread dummy acabar, ou seja, a corrida acabou . */
        pthread_join(thread_dummy[arg->tag - 1], NULL);

    free(pos);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char **argv)
{
    /* Inicializando variáveis locais. */
    uint d = atoi(argv[1]);
    uint n = atoi(argv[2]);
    uint v = atoi(argv[3]);
    uint debug = 0;
    uint final_pos = 1;
    if (argc == 5) debug = 1;
    uint lucky = 0;
    pthread_t thread[n + 1];
    int prob, lucky_cyclist;

    /* Inicializando variáveis globais. */
    is_over = 0;
    n_cyclists = n;
    thread_arg *args = emalloc(n*sizeof(thread_arg));
    thread_report_arg *report_args = emalloc(sizeof(thread_report_arg));
    thread_dummy = emalloc(n*sizeof(pthread_t));
    to_print = createList();
    pista = initializeTrack(d, n);
    pthread_mutex_init(&n_cyclists_mutex, NULL);
    pthread_mutex_init(&check_points, NULL);
    pthread_mutex_init(&sem_output, NULL);

    pthread_barrier_init(&barrier1, NULL ,n + 1);
    pthread_barrier_init(&barrier2, NULL ,n + 1);
    srand(time(NULL));
    ranking = emalloc(n*sizeof(rank));

    for (int i = 0; i < 4; i++)
        points_buffer[i] = 11;

    lucky_cyclist = rand()%n;
    prob = rand()%100;
    if (prob <= 10)
        lucky = 1;

    /* Cria as threads ciclistas. */
    for (uint i = 0; i < n; i++){
        args[i].d = d;
        args[i].v = v;
        args[i].n = n;
        args[i].tag = i + 1;
        if (i == lucky_cyclist)
            args[i].lucky = lucky;
        else
            args[i].lucky = 0;
        pthread_create(&(thread[i]), NULL, &ciclista, &args[i]);
    }

    /* Cria a thread que realiza o Relatório e o debug da corrida. */
    report_args->d = d;
    report_args->n = n;
    report_args->debug = debug;
    pthread_create(&(thread[n]), NULL, &report, report_args);

    /* Espera todos os ciclistas terminarem a prova. */
    for (int i = 0; i < n + 1; i++)
        pthread_join(thread[i], NULL); 

    /* Ordena o ranking para a apresentação final. */
    qsort(ranking, n, sizeof(rank), cmpPts);

    /*Imprime o ranking dos que completaram a prova. */
    for (int i = 0; i < n; i++) {
        if (ranking[i].time_elapsed != -1) { 
            printf("%dº colocado: Ciclista %u terminou com %u pontos em completou a corrida em %.2f segundos\n", 
                    final_pos, ranking[i].tag, ranking[i].pts, ranking[i].time_elapsed);
            if (ranking[i].pts > 0) final_pos++;
        }
    }

    for (int i = 0; i < n; i++) {
        if (ranking[i].time_elapsed == -1){
            printf("Ciclista %u: Quebrou na volta %u.\n", 
                    ranking[i].tag, ranking[i].broken_lap);
        }
    }

    /* Libera a memória usada. */
    free(args);
    free(report_args);
    free(ranking);
    free(thread_dummy);
    destroyTrack(pista, d);
    pthread_mutex_destroy(&n_cyclists_mutex);
    pthread_mutex_destroy(&check_points);
    pthread_mutex_destroy(&sem_output);
    pthread_barrier_destroy(&barrier1);
    pthread_barrier_destroy(&barrier2);

    return 0;
}
