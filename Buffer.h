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
Buffer *create_buffer();
/*esta função recebe um ponteiro para o buffer que sera realocado, ela ira
 *dobrar o tamanho do buffer original, mantendo seu conteudo igual.
 */
void realloc_buffer(Buffer *B);
/*a função recebe um buffer e um char que ira ser adicionado no buffer, a função
 *aumenta o tamanho do buffer se nescessario.
*/
void add_to_buffer(Buffer *B, char c);
/*esta função recebe um buffer e ira desalocar a memoria usada por ele.
 */
void destroy_buffer(Buffer *B);
/*função recebe um buffer e faz com que ele volte para seu estado original.
 */
void clear_buffer(Buffer *B);
/*função recebe o tamanho para allocar um ponteiro e devolve a posição inicial,
 *se houver um erro na alocação a função imprime uma menssagem de erro e acaba o
 *processo.
 */
void *emalloc(size_t size);
#endif
