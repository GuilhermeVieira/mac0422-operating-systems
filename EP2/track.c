// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "track.h"

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
