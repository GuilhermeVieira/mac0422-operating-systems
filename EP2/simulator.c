#include <stdio.h> 	//fprintf().
#include <stdlib.h> //atoi(), malloc(), rand(), srand().
#include <time.h> //time().
#include <pthread.h> //pthreads.

#define LANES 10

typedef unsigned int uint;
typedef struct { uint d, n, v, tag;} thread_arg;
typedef struct { int x, y;} position;

pthread_barrier_t barrier;
pthread_mutex_t track_mutex;
pthread_mutex_t *sem_vec;
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

double velInRefreshTime (double velocity, int refresh)
{
    if (refresh == 2)
        return (velocity/3600)*60;
    else
        return (velocity/3600)*20;
}

int getNewVelocity (int velocity)
{
  	int prob;

    srand(time(NULL));
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

int updatePosition(position *pos, int lenth)
{
  	int new_pos_x = pos->x - 1;
    int new_pos_y = pos->y;
    /*Ve se tem que usar a circularidade da pista*/
    if (new_pos_x < 0)
      	new_pos_x = lenth - 1;
    /*Ve se pode ir imediatamente para frente*/
    if (pista[new_pos_x][pos->y] == 0){
      	pos->x = new_pos_x;
        /*Ir para à esquerda sempre que der*/
        if (pos->y > 0 && pista[new_pos_x][pos->y - 1] == 0)
            pos->y = new_pos_y - 1;
        return 0;
    }
    /*Ver se pode ir para ultrapassar pela direita*/
    else {
        for (; new_pos_y < LANES; new_pos_y++)
            if (pista[pos->x][new_pos_y] == 0 && pista[new_pos_x][new_pos_y] == 0){
                pos->x = new_pos_x;
                pos->y = new_pos_y;
                return 0;
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

void manager()
{

  	return;
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

    pthread_mutex_init(&(sem_vec[arg->tag -1]), NULL);
    pthread_mutex_lock(&(sem_vec[arg->tag -1]));

    while (laps < arg->v){
      	pthread_mutex_lock(&(sem_vec[arg->tag -1]));

        if (!blocked)
  			updatePos = (updatePos + ((int) velInRefreshTime(velocity, refresh)*refresh))%refresh;

        else
          	updatePos = 0;

        if (updatePos == 0){
  			blocked = updatePosition(pos, arg->n);
            if (pos->x == arg->d - 1){
  				velocity = getNewVelocity(velocity);
  				laps++;
  			}
        }
        /*espera todas as threads calcularem sua nova pos*/
        pthread_barrier_wait(&barrier);
        /*só uma thread pode mudar a sua pos por vez, ainda pode bugar, o cara manda pro valor certo e dps um cara manda para um errado*/
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
    pthread_t thread[n];

  	sem_vec = emalloc(n*sizeof(pthread_mutex_t));
  	thread_arg *args = emalloc(n*sizeof(thread_arg));
    pista = initializeTrack(d, n);
	pthread_mutex_init(&track_mutex, NULL);

  	for (uint i = 0; i < n; i++){
      	args[i].d = d;
      	args[i].n = n;
      	args[i].v = v;
      	args[i].tag = i + 1;
      	pthread_create(&(thread[i]), NULL, &ciclista, &args[i]);
    }

    for (int i = 0; i < n; i++)
    	pthread_join(thread[i], NULL);

    destroyTrack(pista, d);

  	return 0;
}
