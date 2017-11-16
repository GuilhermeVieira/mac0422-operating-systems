import math

#A função vê se a página que contem a posição p se encontra na p_mem.
def page_fault(pos, proc, p, p_mem) :
    vit_pos = pos + proc.base*p
    vit_page = math.floor(vit_pos/p)
    if (vit_page in p_mem):
        return 0
    return 1

def OPTIMAL(p_mem, next_pages, proc, pos, p) :
    page = math.floor((pos + proc.base*p)/p)
    if (-1 not in p_mem) :
        best = [-1, -1]
        for i in range(len(p_mem)) :
            for j in range(len(next_pages)) :
                n_page = math.floor((next_pages[j][1] + next_pages[j][0].base*p)/p)
                if (p_mem[i] == n_page) :
                    temp = j
                    if (temp > best[0]) :
                        best[0] = temp
                        best[1] = i
                    break
            if (next_pages and j == len(next_pages) - 1 and p_mem[-1] != page) :
                best[1] = i
                break
        #Se next_pages esta vazia então a página sera carregada na posição -1
        #da p_mem, que é equivalente a ultima posição da p_mem.
        p_mem[best[1]] = page
    else :
        i = p_mem.index(-1)
        p_mem[i] = page

def FIFO(p_mem, indexes, proc, pos, p) :
    page = math.floor((pos + proc.base*p)/p)
    #Coloca a nova página na posição presente no começo da indexes.
    if (-1 not in p_mem) :
        i = indexes.pop(0)
        p_mem[i] = page
        indexes.append(i)
    #Se houver uma posição vazia coloca nela.
    else :
        i = p_mem.index(-1)
        p_mem[i] = page
        indexes.append(i)

def LRU2(p_mem, matrix, proc, pos, p, p_fault) :
    page = math.floor((pos + proc.base*p)/p)
    least = [-1, len(matrix[0])]
    if (p_fault) :
        for i in range(len(matrix)) :
            temp = 0
            for j in range(len(matrix[i])) :
                temp += matrix[i][j]
            if (least[0] == -1 or temp < least[1]) :
                least[0] = i
                least[1] = temp
        p_mem[least[0]] = page
    else :
        least[0] = p_mem.index(page)
    #Atualiza a matriz.
    for j in range(len(matrix[least[0]])) :
        matrix[least[0]][j] = 1
    for j in range(len(matrix[least[0]])) :
        matrix[j][least[0]] = 0

def LRU4(p_mem, count, proc, pos, p, p_fault) :
    #ver mudanças nescessárias para o bit R, falta dar um shift quando dá p_fault.
    page = math.floor((pos + proc.base*p)/p)
    least = [-1, len(count[0])]
    #Atualiza os contadores.
    for i in range(len(count)) :
        for j in range(len(count[i]) - 2, -1, -1) :
            count[i][j + 1] = count[i][j]
    #Procura a página que sera substituida.
    if (p_fault) :
        for i in range(len(count)) :
            temp = 0
            for j in range(1, len(count[i])) :
                temp += count[i][j]
            if (least[0] == -1 or temp < least[1]) :
                least[0] = i
                least[1] = temp
        p_mem[least[0]] = page
        for j in range(len(count[least[0]])) :
            count[least[0]][j] = 0
        count[least[0]][0] = 1
        count[least[0]][1] = 1
