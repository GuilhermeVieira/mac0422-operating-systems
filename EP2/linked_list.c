// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "linked_list.h"

List createList()
{
    List root = NULL;
    return root;
}

List addList(List root, uint lap, uint tag)
{
    if (root == NULL) {
        root = emalloc(sizeof(Cell));
        root->tags_vector = emalloc(n_cyclists*sizeof(uint *));
        root->vector_size = n_cyclists;
        for (int i = 1; i < n_cyclists; i++)
            root->tags_vector[i] = 0;
        root->tags_vector[0] = tag;
        root->top = 1;
        root->lap = lap;
        root->next = NULL;
        return root;
    }
    if (root->lap != lap){
        root->next = addList(root->next, lap, tag);
    }
    else {
        int i;
        for (i = 0; i < root->vector_size && root->tags_vector[i] != tag && 
            root->tags_vector[i] != 0; i++){}
        if (i < root->vector_size && root->tags_vector[i] != tag){
            root->tags_vector[i] = tag;
            root->top++;
        }
    }
    return root;
}

List removeList(List root)
{
    if (root == NULL)
        return root;
    List temp = root->next;
    free(root->tags_vector);
    free(root);
    return temp;
}

List printLap(List root)
{
    if (root == NULL)
        return NULL;
    if (root->lap != 1) {
        printf("Ordem de chegada na volta %u:\n", root->lap - 1);
        for (int i = 0; i < root->top; i++){
            printf("%u ",root->tags_vector[i]);
        }
        printf("\n\n");
    }
    root = removeList(root);
    return root;
}
