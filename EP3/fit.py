import math
from mem import glue_mem

def best_fit(v_mem, proc, s, p) :
    #começar com um best que não existe.
    best = -1
    #acha o número de paginas do processo.
    n_pages = math.ceil((math.ceil((proc.b)/s)*s)/p)
    for i in range(len(v_mem)) :
        #procura uma posição sem ninguém.
        if (v_mem[i][0] == -1) :
            #ve se tem espaço suficiente.
            if ((v_mem[i][2] - v_mem[i][1]) >= n_pages) :
                if (best == -1) :
                    best = i
                #ve se precisa mudar o best.
                elif (v_mem[i][2] - v_mem[i][1] < v_mem[best][2] - v_mem[best][1]) :
                    best = i
        else :
            continue
    #Se tiver espaço sobrando marca como vazio
    if (v_mem[best][2] - v_mem[best][1] > n_pages) :
        v_mem.insert(best + 1 ,[-1, v_mem[best][1] + n_pages, v_mem[best][2]])
    v_mem[best][0], v_mem[best][2] = proc.pid, v_mem[best][1] + n_pages
    proc.new_base(v_mem[best][1])
    proc.new_top(v_mem[best][2])
    glue_mem(v_mem)
    return best

def worst_fit(v_mem, proc, s, p) :
    #começar com um worst que não existe.
    worst = -1
    #acha o número de paginas usadas pelo processo.
    n_pages = math.ceil((math.ceil((proc.b)/s)*s)/p)
    for i in range(len(v_mem)) :
        #procura uma posição sem ninguém.
        if (v_mem[i][0] == -1) :
            #ve se tem espaço suficiente.
            if ((v_mem[i][2] - v_mem[i][1]) >= n_pages) :
                if (worst == -1) :
                    worst = i
                #ve se precisa mudar o worst.
                elif (v_mem[i][2] - v_mem[i][1] > v_mem[worst][2] - v_mem[worst][1]) :
                    worst = i
        else :
            continue
    if (v_mem[worst][2] - v_mem[worst][1] > n_pages) :
        v_mem.insert(worst + 1 ,[-1, v_mem[worst][1] + n_pages, v_mem[worst][2]])
    v_mem[worst][0], v_mem[worst][2] = proc.pid, v_mem[worst][1] + n_pages
    proc.new_base(v_mem[worst][1])
    proc.new_top(v_mem[worst][2])
    glue_mem(v_mem)
    return

def quick_fit(v_mem, proc, s, p, qf_sizes, available) :
    n_pages = math.ceil((math.ceil((proc.b)/s)*s)/p)
    if (n_pages in qf_sizes) :
        i = qf_sizes.index(n_pages)
        pos = available[i][0]
        if (v_mem[pos][2] - v_mem[pos][1] > n_pages) :
            v_mem.insert(pos + 1 ,[-1, v_mem[pos][1] + n_pages, v_mem[pos][2]])
        v_mem[pos][0], v_mem[pos][2] = proc.pid, v_mem[pos][1] + n_pages
        proc.new_base(v_mem[pos][1])
        proc.new_top(v_mem[pos][2])
        glue_mem(v_mem) #Se der errado, lembrar que isso pode não estar aqui
        fix_available(available, pos, qf_sizes, v_mem) #dá pop em todas as listas com pos
    else :
        pos = best_fit(v_mem, proc, s, p)
        fix_available(available, pos, qf_sizes, v_mem)

def fix_available(available, pos, qf_sizes, v_mem) :
    for i in range(len(available)) :
        to_pop = 0
        #quem sabe pode bugar quando a available[i] for vazia
        for j in range(len(available[i])) :
            if (available[i][j] == pos) :
                to_pop += 1
        for j in range(to_pop) :
            available[i].remove(pos)
        if (pos + 1 != len(v_mem) and v_mem[pos + 1][0] == -1 and v_mem[pos + 1][2] - v_mem[pos + 1][1] >= qf_sizes[i]) :
            for j in range((v_mem[pos + 1][2] - v_mem[pos + 1][1])//qf_sizes[i]) :
                available[i].insert(0, pos + 1)

def find_available(v_mem, qf_sizes) :
    available = []
    for k in qf_sizes :
        available.append([])
        for i in range(len(v_mem)) :
            if (v_mem[i][2] - v_mem[i][1] >= k) :
                for j in range((v_mem[i][2] - v_mem[i][1])//k) :
                    available[-1].append(i)
    return available
