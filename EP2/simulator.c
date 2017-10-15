// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include <stdlib.h> //atoi(), rand(), srand().
#include <time.h> //time().
#include <pthread.h> //pthreads.
#include "globals.h"
#include "sort.h"
#include "track.h"
#include "linked_list.h"

typedef struct { uint d, n, v, tag, lucky; } thread_arg;
typedef struct { uint d, n, debug; } thread_report_arg;

pthread_mutex_t n_cyclists_mutex;
pthread_mutex_t check_points;
pthread_mutex_t sem_output;

pthread_barrier_t barrier1;
pthread_barrier_t barrier2;
pthread_t *thread_dummy;

int refresh = MS60;
List to_print;

/*Função procura elemento x em um vetor temp de tamanho n. Retorna 1 se x se
 *encontra em temp, 0 caso não se encontre.
 */
int inArray (rank *temp, uint x, uint n)
{
    uint i = 0;
    for (; i < n && temp[i].laps != x; i++){}
    if (i != n && temp[i].laps == x){
        return 1;
    }
    return 0;
}

/*Dado o némero de voltas laps a funçã retorna se o ciclista quebrou ou não ao
 *completar a volta.
 */
int break_cyclists(uint laps)
{
    int prob = rand()%100;
    if (laps%15 == 0 && prob <= 1){
        pthread_mutex_lock(&n_cyclists_mutex);
        if (n_cyclists <= 5){
            pthread_mutex_unlock(&n_cyclists_mutex);
            return 0;
        }
        pthread_mutex_unlock(&n_cyclists_mutex);
        return 1;
    }
    return 0;
}

/*Dado uma velocidade em Km/h a função ira retornar ela em (m/ms)*dt, onde dt
 *esta está relacionado com o refresh.
 */
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

/*Função retorna a nova velocidade de um ciclista dado sua velocidade antiga.
 */
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

/*Dado uma pista global, seu tamanho, e as coordenadas do ciclista que ira se
 *mover (dado por pos). A função ira calcular sua nova posição e ira mudar o
 *argumento pos de acordo, a função retorna 1 se foi póssivel mover o ciclista e
 * 0 se ele estava impedido de fazer qualquer movimento.
 */
int updatePosition(position *pos, int length)
{
	position *old_pos = emalloc(sizeof(position));
	old_pos->x = pos->x;
    old_pos->y = pos->y;
    int new_pos_x = pos->x - 1;
    int new_pos_y = pos->y;
    int ret = 0;
    /*Ve se tem que usar a circularidade da pista*/
    if (new_pos_x < 0)
        new_pos_x = length - 1;
    int temp = new_pos_x;
    int temp2 = pos->y;
    /* Vê o cara de cima */
    pthread_mutex_lock(&pista[new_pos_x][pos->y].mutex);
    if (pista[new_pos_x][pos->y].pos == 0) {
    	/* Vai pra esquerda e pra frente (diagonal) ultimo if verifica se n é ultrapassagem pela interna */
	    for (int j = pos->y -1; j >= 0; j--) {
	    	if (pista[pos->x][j].pos == 0) {
		    	pthread_mutex_lock(&pista[new_pos_x][j].mutex);
		    	if (pista[new_pos_x][j].pos == 0){
		    		new_pos_y = j;
                    break;
                }
		    	pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
	    	}
	    	else
	    		break;
	    }
	    /* Não deu pra ir pra + interna, vou pra frente */
        pos->x = new_pos_x;
        pos->y = new_pos_y;
        /*não pode se mover para nenhum lugar então tentara se mover na prox iteração*/
        updateTrack(pos, old_pos); // Atualiza a posição na pista.
        pthread_mutex_unlock(&pista[temp][new_pos_y].mutex);
        pthread_mutex_unlock(&pista[new_pos_x][temp2].mutex);
	}
	else {
		/*tento ultrapassar */
		for (int j = pos->y +1; j < LANES; j++) {
			if (pista[pos->x][j].pos == 0) {
				pthread_mutex_lock(&pista[new_pos_x][j].mutex);
				if (pista[new_pos_x][j].pos == 0) {
					new_pos_y = j;
					break;
				}
				pthread_mutex_unlock(&pista[new_pos_x][j].mutex);
			}
			else
				break;
		}
		if (new_pos_y == old_pos->y)
        	new_pos_x = pos->x;
        pos->x = new_pos_x;
        pos->y = new_pos_y;
        /*não pode se mover para nenhum lugar então tentara se mover na prox iteração*/
        updateTrack(pos, old_pos); // Atualiza a posição na pista.
        pthread_mutex_unlock(&pista[new_pos_x][new_pos_y].mutex);
        pthread_mutex_unlock(&pista[temp][temp2].mutex);
	}


    pthread_mutex_unlock(&pista[old_pos->x][old_pos->y].mutex); //Unlock na pos antiga

    if (new_pos_x == old_pos->x && new_pos_y == old_pos->y) ret = 1;
    free(old_pos);

 	return ret;
}

void *report(void *args)
{
    thread_report_arg *arg = (thread_report_arg *) args;
    rank *temp = emalloc(arg->n*sizeof(rank));
    int first, last;
    uint first_palce = 0;
    int laps_to_points = 1;
    uint last_lap = 0;
    while (1) {
        pthread_barrier_wait(&barrier1);

        for (int i = 0; i < arg->n; i++){
            temp[i].laps = ranking[i].laps;
            temp[i].pos = ranking[i].pos;
            temp[i].pts = ranking[i].pts;
            temp[i].tag = ranking[i].tag;
            temp[i].time_elapsed = ranking[i].time_elapsed;
            temp[i].broken_lap = ranking[i].broken_lap;
        }

        qsort(temp, arg->n, sizeof(rank), cmpLaps);

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
        if (first_palce != temp[0].tag){
            first_palce = temp[0].tag;
            laps_to_points = 1;
        }
        if (temp[0].laps > temp[1].laps + laps_to_points && temp[0].pos.x < temp[1].pos.x){
            ranking[0].pts += 20;
            laps_to_points++;
        }

        if (!inArray(temp, last_lap, arg->n)){
            pthread_mutex_lock(&sem_output);
            to_print = printLap(to_print);
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
        /* Imprime a matriz */
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

        if (is_over) break;
        pthread_barrier_wait(&barrier2);
    }

    free(temp);
    pthread_exit(NULL);
    return NULL;
}

void *dummy()
{
    while (1) {
        pthread_barrier_wait(&barrier1); //espera todo mundo atualizar sua pos
        if (is_over) break;
        pthread_barrier_wait(&barrier2);
    }
    pthread_exit(NULL);
    return NULL;
}

void *ciclista(void *args)
{
    thread_arg *arg = (thread_arg *) args;
    int broken = 0;
    int velocity = 30;
    int updatePos = 0;
    int blocked = 0; // Vale 1 se o ciclista frente está bloqueando a passagem.
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
    /* Loop que simula a corrida. */
    while (laps <= arg->v && !broken){
        if (refresh == MS60)
            ranking[arg->tag - 1].time_elapsed += 0.06;
        else
            ranking[arg->tag - 1].time_elapsed += 0.02;


        /* Calcula a nova posição. */
        if (!blocked) {
            updatePos = (updatePos + ((int) (velInRefreshTime(velocity, refresh)*refresh)))%refresh;
        }
        else {
            updatePos = 0;
        }

        if (updatePos == 0) {
            blocked = updatePosition(pos, arg->d);
            if (pos->x == arg->d - 1) {
                if (laps != 0)
                    velocity = getNewVelocity(velocity);
                laps++;		// CHECAR SE É GARANTIDO QUE O CARA SE MOVEU!

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
        	pthread_mutex_unlock(&pista[pos->x][pos->y].mutex);
        }
        ranking[arg->tag -1].laps = laps;
        ranking[arg->tag -1].pos = *pos;

        pthread_barrier_wait(&barrier1); // Espera todo mundo calcular sua posição.

        pthread_mutex_lock(&pista[pos->x][pos->y].mutex);

        if (arg->lucky == 1 && (arg->v - laps) == 2){
            velocity = 90;
            refresh = MS60;
        }

        if (broken) {
            pthread_mutex_unlock(&pista[pos->x][pos->y].mutex);
            rank *temp = emalloc(arg->n*sizeof(rank));
            for (int i = 0; i < arg->n; i++)
                temp[i] = ranking[i];
            qsort(temp, arg->n, sizeof(rank), cmpPts);
            for (int i = 0; i < arg->n; i++) {
                if (temp[i].tag == arg->tag) {
                    pthread_mutex_lock(&sem_output);
                    printf("O corredor %u quebrou na volta %u e ele estava na posição %d\n\n", arg->tag, ranking[arg->tag -1].broken_lap, i+1);
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
        pthread_barrier_wait(&barrier2); // Espera todo mundo atualizar sua posição na pista.
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
        pthread_join(thread[i], NULL); // ACHO QUE TEM QUE ESPERAR A REPORT TAMBÉM!!!

    /*Imprime o ranking */
    for (int i = 0; i < n; i++){
        printf("ola eu sou o ciclista %u e tenho %u pts ", ranking[i].tag, ranking[i].pts);
        if (ranking[i].time_elapsed == -1){
            printf("e eu quebrei na volta %u \n", ranking[i].broken_lap);
        }
        else{
            printf("e completei a corrida em %.2f segundos\n", ranking[i].time_elapsed);
        }
    }
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
