#include <stdio.h>  //fprintf().
#include <stdlib.h> //atoi(), malloc(), rand(), srand().
#include <time.h> //time().
#include <pthread.h> //pthreads.
#include <unistd.h> //

#define LANES 10

typedef unsigned int uint;
typedef struct { uint d, v, tag;} thread_arg;
typedef struct { int x, y;} position;

pthread_mutex_t track_mutex;
pthread_mutex_t n_cyclists_mutex;

pthread_barrier_t barrier1;
pthread_barrier_t barrier2;
pthread_t *thread_dummy;
uint n_cyclists = 0;
uint **pista;
int is_over;

void *emalloc(size_t size)
{
    void *pointer;
    pointer = malloc(size);

    if (pointer == NULL) {
        fprintf(stderr,
                "ERRO: Não foi possível alocar memória suficiente.\n");
        exit(EXIT_FAILURE);
    }

    else
        return pointer;
}

void *report(void *arg)
{
    uint d = *((uint *) arg);
    while (1) {
      pthread_barrier_wait(&barrier1);

      if (n_cyclists > 0) {
            for (int i = 0; i < d; i++) {
               for (int j = 0; j < LANES; j++)
                    printf("%d ", pista[i][j]);
                printf("\n");
            }
            printf("\n");
      }

      if (is_over) break;
      pthread_barrier_wait(&barrier2);
    }

    return NULL;
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

uint **initializeTrack(uint d, uint n)
{
    uint **new_track;
    uint tag = 1;
    new_track = emalloc(d*sizeof(uint *));

    for (int i = 0; i < d; i++){
        new_track[i] = emalloc(LANES*sizeof(uint));

        for (int j = 0; j < LANES; j++){

            if (tag <= n){
                new_track[i][j] = tag;
                tag++;
            }

            else
                new_track[i][j] = 0;
        }
    }
    return new_track;
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

void updateTrack(position *pos, position *old_pos, uint tag)
{
    pista[old_pos->x][old_pos->y] = 0;
    pista[pos->x][pos->y] = tag;
    return;
}

void destroyTrack(uint **track, uint d)
{
    for (int i = 0; i < d; i++)
        free(track[i]);
    free(track);
    return;
}

void *dummy()
{
    while (1){
        pthread_barrier_wait(&barrier1); //espera todo mundo atualizar sua pos
        if (is_over) break;
        pthread_barrier_wait(&barrier2);
    }
    printf("SAI\n");
    return NULL;
}

void *ciclista(void *args)
{
    thread_arg *arg = (thread_arg *) args;
    int velocity = 30;
    int refresh = 2;
    int updatePos = 0;
    int blocked = 0; // Vale 1 se o ciclista frente está bloqueando a passagem.
    uint laps = 0;
    position *pos = emalloc(sizeof(position));
    position *old_pos = emalloc(sizeof(position));
    pos->x = (arg->tag - 1)/10;
    pos->y = (arg->tag - 1)%10;
    
    /* Loop que simula a corrida. */
    while (laps <= arg->v){
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
            }
        }

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
    pthread_t thread[n + 1];

    /* Inicializando variáveis globais. */
    is_over = 0;
    n_cyclists = n;
    thread_arg *args = emalloc(n*sizeof(thread_arg));
    thread_dummy = emalloc(n*sizeof(pthread_t));
    pista = initializeTrack(d, n);
    pthread_mutex_init(&n_cyclists_mutex, NULL);
    pthread_barrier_init(&barrier1, NULL ,n+1); 
    pthread_barrier_init(&barrier2, NULL ,n+1); 
    srand(time(NULL));
    
    /* Cria as threads ciclistas. */
    for (uint i = 0; i < n; i++){
        args[i].d = d;
        args[i].v = v;
        args[i].tag = i + 1;
        pthread_create(&(thread[i]), NULL, &ciclista, &args[i]);
    }

    /* Cria a thread que realiza o Relatório e o debug da corrida. */
    pthread_create(&(thread[n + 1]), NULL, &report, &d);
    
    /* Espera todos os ciclistas terminarem a prova. */
    for (int i = 0; i < n; i++)
        pthread_join(thread[i], NULL); // ACHO QUE TEM QUE ESPERAR A REPORT TAMBÉM!!!

    free(args);
    destroyTrack(pista, d);

    return 0;
}
