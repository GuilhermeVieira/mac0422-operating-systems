// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include <stdlib.h> //atoi(), rand(), srand().
#include <time.h> //time().
#include <pthread.h> //pthreads.
#include "track.h" //emalloc(), initializeTrack(), updateTrack(), destroyTrack().

#define MS60  2
#define MS20  6

typedef struct { uint d, v, tag, lucky;} thread_arg;
typedef struct { uint d, n, debug;} thread_report_arg;

pthread_mutex_t n_cyclists_mutex;
pthread_barrier_t barrier1;
pthread_barrier_t barrier2;
pthread_t *thread_dummy;

uint n_cyclists = 0;
int refresh = MS60;
int is_over;
position *all_pos;
uint *all_laps;

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

int updatePosition(position *pos, int length)
{
    int new_pos_x = pos->x - 1;
    int new_pos_y = pos->y;
    /*Ve se tem que usar a circularidade da pista*/
    if (new_pos_x < 0)
        new_pos_x = length - 1;

    /* Vai pra esquerda e pra frente (diagonal) ultimo if verifica se n é ultrapassagem pela interna */
    if (pos->y > 0 && pista[pos->x][pos->y - 1] == 0 && pista[new_pos_x][pos->y - 1] == 0 && pista[new_pos_x][pos->y] == 0){
        pos->x = new_pos_x;
        pos->y = new_pos_y - 1;
        return 0;
    }

    /*Ve se pode ir imediatamente para frente*/
    if (pista[new_pos_x][pos->y] == 0){
        pos->x = new_pos_x;
        return 0;
    }
    /*Ver se pode ir para ultrapassar pela direita*/
    else {
        for (; new_pos_y < LANES; new_pos_y++){
            if (pista[pos->x][new_pos_y] == 0)
                if (pista[new_pos_x][new_pos_y] == 0){
                    pos->x = new_pos_x;
                    pos->y = new_pos_y;
                    return 0;
                }
                else
                    continue;
            else
                break;
        }
    }
    /*não pode se mover para nenhum lugar então tentara se mover na prox iteração*/
    return 1;
}

void *report(void *args)
{
    thread_report_arg *arg = (thread_report_arg *) args;
    
    while (1) {
        pthread_barrier_wait(&barrier1);
        printf("\n");
        for (int i = 0; i < arg->n; i++) 
        	printf("%u ", all_laps[i]);
        printf("\n");

        if (is_over) break;
        pthread_barrier_wait(&barrier2);
        
        /* Imprime a matriz */
        if (arg->debug && n_cyclists > 0) {
            for (int i = 0; i < arg->d; i++) {
                for (int j = 0; j < LANES; j++)
                    printf("%d ", pista[i][j]);
                printf("\n");
            }
            printf("\n");
        }
    }
    return NULL;
}

void *dummy()
{
    while (1){
        pthread_barrier_wait(&barrier1); //espera todo mundo atualizar sua pos
        if (is_over) break;
        pthread_barrier_wait(&barrier2);
    }
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
    position *old_pos = emalloc(sizeof(position));
    pos->x = (arg->tag - 1)/10;
    pos->y = (arg->tag - 1)%10;

    /* Loop que simula a corrida. */
    while (laps <= arg->v && !broken){
        old_pos->x = pos->x;
        old_pos->y = pos->y;

        /* Calcula a nova posição. */
        if (!blocked) {
            updatePos = (updatePos + ((int) (velInRefreshTime(velocity, refresh)*refresh)))%refresh;
        }
        else {
            updatePos = 0;
        }
        if (updatePos == 0) {
            blocked = updatePosition(pos, arg->d);
            if (pos->x == arg->d - 1){
                velocity = getNewVelocity(velocity);
                laps++;
                if (arg->lucky == 1 && (arg->v - laps) == 2){
                    velocity = 90;
                    refresh = MS60;
                }
                broken = break_cyclists(laps);
                if (broken){
                    printf("O corredor %u quebrou na volta %u e ele estava na posição \n", arg->tag, laps);
                }
            }
        }
        all_laps[arg->tag -1] = laps;
        all_pos[arg->tag -1] = *pos;

        pthread_barrier_wait(&barrier1); // Espera todo mundo calcular sua posição.
        updateTrack(pos, old_pos, arg->tag); // Atualiza a posição na pista.
        pthread_barrier_wait(&barrier2); // Espera todo mundo atualizar sua posição na pista.
    }

    /* Tira o ciclista da pista. */
    pista[pos->x][pos->y] = 0; // ACHO QUE PRECISA COLOCAR UMAS VERIFICAÇOES PARA NAO SOBRESCREVER

    /* Atualiza o número de ciclistas na pista. */
    pthread_mutex_lock(&n_cyclists_mutex);
    n_cyclists--;
    pthread_mutex_unlock(&n_cyclists_mutex);

    /* Verifica se a corrida acabou e manda um sinal para todos pararem. */
    if (n_cyclists < 1) is_over = 1;

    /* Cria uma thread dummy para deixar a barreira funcional. */
    pthread_create(&(thread_dummy[arg->tag - 1]), NULL, &dummy, NULL);

    /* Espera a thread dummy acabar, ou seja, a corrida acabou . */
    pthread_join(thread_dummy[arg->tag - 1], NULL);

    free(pos);
    free(old_pos);

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
    pista = initializeTrack(d, n);
    pthread_mutex_init(&n_cyclists_mutex, NULL);
    pthread_barrier_init(&barrier1, NULL ,n+1);
    pthread_barrier_init(&barrier2, NULL ,n+1);
    srand(time(NULL));
    all_laps = emalloc(n*sizeof(uint));
	all_pos = emalloc(n*sizeof(position));

    lucky_cyclist = rand()%n;
    prob = rand()%100;
    if (prob <= 10)
        lucky = 1;

    /* Cria as threads ciclistas. */
    for (uint i = 0; i < n; i++){
        args[i].d = d;
        args[i].v = v;
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
    pthread_create(&(thread[n + 1]), NULL, &report, report_args);

    /* Espera todos os ciclistas terminarem a prova. */
    for (int i = 0; i < n; i++)
        pthread_join(thread[i], NULL); // ACHO QUE TEM QUE ESPERAR A REPORT TAMBÉM!!!

    free(args);
    destroyTrack(pista, d);

    return 0;
}
