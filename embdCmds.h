#ifndef __EMBDCMDS_H__
#define __EMBDCMDS_H__

/*função sem srgumentos que imprime na saida padrão o dia da semana,mes,dia,
 *horario,
 */
void embdDate();

/*Função recebe um vetor contendo os argumentos da syscall chown() e aplica eles
 *a função.
 */
void embdChown(char **commands);

#endif
