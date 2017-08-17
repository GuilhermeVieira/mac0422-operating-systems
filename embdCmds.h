#ifndef __EMBDCMDS_H__
#define __EMBDCMDS_H__

/*função sem srgumentos que imprime na saida padrão o dia da semana,mes,dia,
 *horario,
 */
void embd_date();

/*Função recebe um vetor contendo os argumentos da syscall chown() e aplica eles
 *a função.
 */
void embd_chown(char **commands);

#endif
