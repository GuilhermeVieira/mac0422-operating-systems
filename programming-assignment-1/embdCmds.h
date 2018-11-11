// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#ifndef __EMBDCMDS_H__
#define __EMBDCMDS_H__

#include <stdio.h> // printf(), fprintf().
#include <stdlib.h> // free(), sizeof().
#include <string.h> // strcmp(), strlen().
#include <time.h> // tm, time(), ctime(), localtime().
#include <unistd.h> // chown().
#include <pwd.h> // passwd, getpwnam().
#include <grp.h> // group, getgrnam().
#include "buffer.h" // Implementa as funções de um buffer simples.

// Imprime na saída padrão o dia da semana, mês, dia do mês, horário e ano.
void embdDate();

/* Recebe um vetor contendo os argumentos da syscall chown() e realiza essa
   chamada de sistema com esses argumentos. */
void embdChown(char **commands);

#endif
