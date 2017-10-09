#include <stdio.h> 	//fprintf().
#include <stdlib.h> //atoi(), malloc(), rand(), srand().
#include <time.h> //time().
#include <pthread.h> //pthreads.
#include <unistd.h>

#define LANES 10

typedef unsigned int uint;
typedef struct { uint d, v, tag;} thread_arg;
typedef struct { uint d, v;} manager_args;
typedef struct { int x, y;} position;

pthread_barrier_t barrier;
pthread_mutex_t track_mutex;
pthread_mutex_t *sem_vec;
uint n_cyclists = 0;
uint **pista;

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

/* Recebe um ponteiro pos de um ciclista e o tamanho da pista. Se conseguir achar uma posição nova para
 * o ciclista, atualiza o ponteiro pos com a nova posição e retorna 0. Caso o ciclista seja bloqueado,
 * retorna 1. */
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

int hasBeenSimulated(uint *already_simulated, int x)
{
	for (int i = 0; i < LANES; i++)
      	if (already_simulated[i] == x)
      		return 1;
    return 0;
}

void *manager(void *args)
{
    manager_args *arg;
    arg = (manager_args *) args;
	uint aligned_cyclists[LANES];
  	uint already_simulated[LANES];
  	int count; // nº de ciclistas numa linha

  	/* inicializa o vetor com 0 */
  	for (int i = 0; i < LANES; i++)
		aligned_cyclists[i] = already_simulated[i] = 0;
    while (n_cyclists > 0) {
        /*todo mundo é simulado UMA vez, não ser que ele vai da 0 para n - 1*/
        for (int i = 0; i < arg->d; i++){
            count = 1;
            for (int j = 0; j < LANES; j++){
              	if (i == 0)
					already_simulated[j] = pista[i][j];
              	/* se a  gente já simulou esse cara antes */
                if (i == arg->d - 1 && hasBeenSimulated(already_simulated, pista[i][j]) == 1){
                      aligned_cyclists[j] = 0;
                }
              	else
                  	aligned_cyclists[j] = pista[i][j];

                if (aligned_cyclists[j] != 0){
                    count++;
                }
    		}
          	pthread_barrier_init(&barrier, NULL, count);

          	for (int i = 0; i < LANES; i++){
      			if (aligned_cyclists[i] != 0)
                    pthread_mutex_unlock(&(sem_vec[aligned_cyclists[i] - 1]));
       		}
            pthread_barrier_wait(&barrier);
        	pthread_barrier_wait(&barrier);
            for (int i = 0; i < arg->d; i++){
                for (int j = 0; j < LANES; j++)
                    printf("%d ", pista[i][j]);
                printf("\n");
            }
            printf("\n");
    	}
    }
  	return NULL;
}


void *ciclista(void *args)
{
    thread_arg *arg = (thread_arg *) args;
	int updatePos = 0;
	int refresh = 2;
  	int blocked = 0; // Vale 1 se o ciclista frente está bloqueando a passagem.
    int velocity = 30;
    uint laps = 0;
  	position *pos = emalloc(sizeof(position));
    position *old_pos = emalloc(sizeof(position));
  	pos->x = old_pos->x = (arg->tag - 1)/10;
  	pos->y = old_pos->y = (arg->tag - 1)%10;
    while (laps < arg->v){
        pthread_mutex_lock(&(sem_vec[arg->tag -1]));
        old_pos->x = pos->x;
        old_pos->y = pos->y;
        if (!blocked){
  			updatePos = (updatePos + ((int) (velInRefreshTime(velocity, refresh)*refresh)))%refresh;
        }
        else
          	updatePos = 0;

        if (updatePos == 0){
  			blocked = updatePosition(pos, arg->d);
            if (pos->x == arg->d - 1){
  				velocity = getNewVelocity(velocity);
  				laps++;
  			}
        }
        /*espera todas as threads calcularem sua nova pos*/
        pthread_barrier_wait(&barrier);
        /*só uma thread pode mudar a sua pos por vez*/
        pthread_mutex_lock(&track_mutex);
        updateTrack(pos, old_pos, arg->tag);
        pthread_mutex_unlock(&track_mutex);
        /*espera todo mundo mudar sua pos*/
        pthread_barrier_wait(&barrier);
  	}
  	return NULL;
}

int main(int argc, char **argv)
{
  	uint d = atoi(argv[1]);
  	uint n = atoi(argv[2]);
  	uint v = atoi(argv[3]);
    pthread_t thread[n + 1];

    n_cyclists = n;
  	sem_vec = emalloc(n*sizeof(pthread_mutex_t));
  	thread_arg *args = emalloc(n*sizeof(thread_arg));
    manager_args *m_args = emalloc(sizeof(manager_args));
  	pista = initializeTrack(d, n);
	pthread_mutex_init(&track_mutex, NULL);
    srand(time(NULL));
  	for (uint i = 0; i < n; i++){
      	args[i].d = d;
      	args[i].v = v;
      	args[i].tag = i + 1;
        pthread_mutex_init(&(sem_vec[i]), NULL);
        pthread_mutex_lock(&(sem_vec[i]));
      	pthread_create(&(thread[i]), NULL, &ciclista, &args[i]);
    }
    m_args->d = d;
    m_args->v = v;
  	pthread_create(&thread[n], NULL, &manager, m_args);
    pthread_join(thread[n], NULL);

    destroyTrack(pista, d);

  	return 0;
}
