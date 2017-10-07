#include <stdio.h> 	//fprintf.
#include <stdlib.h> //atoi, malloc.
#include <pthread.h> //pthreads.

#define LANES 10

typedef unsigned int uint;

typedef struct { uint d, n, v, tag;} thread_arg;
typedef struct { int x, y;} position;

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
    else return pointer;
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

void updateTrack(position *pos) {
  
  
}

void destroyTrack(uint **track, uint d)
{
  	for (int i = 0; i < d; i++)
  		free(track[i]);
	free(track);
  	return;
}


void *ciclista(void *args)
{
	int bob = 0;
	int refresh = 2;
  	thread_arg *arg = (thread_arg *) args;
  	uint laps = 0;
  	position *pos = emalloc(sizeof(position));
  	pos->x = (arg->tag - 1)/10;
  	pos->y = (arg->tag - 1)%10;
  	double velocity = 1.0/2;
  	while (laps < arg->v){
  		bob += (velocity*refresh)%refresh;
  		if (bob == 0)
  			/*atualiza a pos*/	
  		if (pos.x == d -1){
  			/*sorteia nova velo*/
  			laps++;
  		}
  		/*barreira*/
  	}
  	return NULL;
}

int main(int argc, char **argv)
{
  	uint d = atoi(argv[1]);
  	uint n = atoi(argv[2]);
  	uint v = atoi(argv[3]);

  	pista = initializeTrack(d, n);
  	thread_arg *args = emalloc(n*sizeof(thread_arg));

  	pthread_t thread[n];
  
  	for (uint i = 0; i < n; i++){
      	args[i].d = d;
      	args[i].n = n;
      	args[i].v = v;
      	args[i].tag = i + 1;
      	pthread_create(&(thread[i]), NULL, &ciclista, &args[i]);
    }
    for (int i = 0; i < d; i++){
    	for (int j = 0; j < 10; j++)
    		printf("%u ", pista[i][j]);
    	printf("\n");
    }
    for (int i = 0; i < n; i++){
    	pthread_join(thread[i], NULL);		
    }
    destroyTrack(pista, d);

  	return 0;
}
