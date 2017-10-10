#include <stdio.h> 	//fprintf().
#include <stdlib.h> //atoi(), malloc(), rand(), srand().
#include <time.h> //time().
#include <pthread.h> //pthreads.
#include <unistd.h> //

#define LANES 10

typedef unsigned int uint;
typedef struct { uint d, v, tag;} thread_arg;
typedef struct { int x, y;} position;

pthread_mutex_t track_mutex;
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

void printMatrix(uint d)
{
    for (int i = 0; i < d; i++){
       for (int j = 0; j < LANES; j++)
            printf("%d ", pista[i][j]);
        printf("\n");
    }
    printf("\n");
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

int main(int argc, char **argv)
{
  	uint d = atoi(argv[1]);
  	uint n = atoi(argv[2]);
  	uint v = atoi(argv[3]);
    pthread_t thread[n];

    n_cyclists = n;
  	thread_arg *args = emalloc(n*sizeof(thread_arg));
  	pista = initializeTrack(d, n);
	pthread_mutex_init(&track_mutex, NULL);
    srand(time(NULL));
  	for (uint i = 0; i < n; i++){
      	args[i].d = d;
      	args[i].v = v;
      	args[i].tag = i + 1;
      	pthread_create(&(thread[i]), NULL, &ciclista, &args[i]);
    }
    for (int i = 0; i < n; i++)
        pthread_join(thread[i], NULL);

    destroyTrack(pista, d);

  	return 0;
}
