// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "sort.h"

int cmpLaps(const void *a, const void *b)
{
    if ((*(rank *) a).laps < (*(rank *) b).laps) return 1;
    else if ((*(rank *) a).laps == (*(rank *) b).laps) return 0;
    return -1;
}

int cmpPos(const void *a, const void *b)
{
    if ((*(rank *) a).pos.x < (*(rank *) b).pos.x) return -1;
    else if ((*(rank *) a).pos.x == (*(rank *) b).pos.x) return 0;
    return 1;
}

int cmpPts(const void *a, const void *b)
{
    if ((*(rank *) a).pts < (*(rank *) b).pts) return 1;
    else if ((*(rank *) a).pts == (*(rank *) b).pts) return 0;
    return -1;
}

void sort_range_array(rank *array, int beginning, int end)
{
	rank *temp = emalloc((end - beginning + 1)*sizeof(rank));
	for (int l = 0 ,k = beginning; k < end + 1; k++, l++)
		temp[l] = array[k];
	qsort(temp, end - beginning + 1, sizeof(rank), cmpPos);
	for (int k = 0; k < end - beginning + 1; k++)
		array[beginning + k] = temp[k];
	free(temp);
	return;
}
