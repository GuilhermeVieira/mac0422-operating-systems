// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "track.h"

square **initializeTrack(uint d, uint n)
{
    square **new_track;
    uint tag = 1;
    new_track = emalloc(d*sizeof(square *));
    pthread_mutex_init(&track_mutex, NULL);

    for (int i = 0; i < d; i++){
        new_track[i] = emalloc(LANES*sizeof(square));

        for (int j = 0; j < LANES; j++){
            pthread_mutex_init(&new_track[i][j].mutex, NULL);
            if (tag <= n){
                new_track[i][j].pos = tag;
                tag++;
                pthread_mutex_lock(&new_track[i][j].mutex);
            }

            else
                new_track[i][j].pos = 0;
        }
    }
    return new_track;
}

void updateTrack(position *pos, position *old_pos)
{
    uint tag;
    pthread_mutex_lock(&track_mutex);
    tag = pista[old_pos->x][old_pos->y].pos;
    pista[old_pos->x][old_pos->y].pos = 0;
    pista[pos->x][pos->y].pos = tag;
    pthread_mutex_unlock(&track_mutex);
    return;
}

void destroyTrack(square **track, uint d)
{
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < LANES; j++)
            pthread_mutex_destroy(&track[i][j].mutex);
        free(track[i]);
    }
    free(track);
    return;
}
