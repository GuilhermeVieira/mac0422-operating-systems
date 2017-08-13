#ifndef __BUFFER_H__
#define __BUFFER_H__
typedef struct {
                char *palavra;
                int max;
                int top;
                }Buffer;

/*esta função cria um buffer(vetor dinamico) de char e devolve um ponteiro para
 *o buffer criado.
*/
Buffer *createBuffer();
/*esta função recebe um ponteiro para o buffer que sera realocado, ela ira
 *dobrar o tamanho do buffer original, mantendo seu conteudo igual.
 */
void reallocBuffer(Buffer *B);
/*a função recebe um buffer e um char que ira ser adicionado no buffer, a função
 *aumenta o tamanho do buffer se nescessario.
*/
void addToBuffer(Buffer *B, char c);
/*esta função recebe um buffer e ira desalocar a memoria usada por ele.
 */
void destroyBuffer(Buffer *B);
/*função recebe um buffer e faz com que ele volte para seu estado original.
 */
void clearBuffer(Buffer *B);
#endif
