import sys
import math

class process:
    def __init__(self, t0, tf, b, name, pid) :
        self.t0 = t0
        self.tf = tf
        self.b = b
        self.name = name
        self.times = []
        self.pid = pid
    def addExec(self, px, tx) :
        self.times.append((px, tx))
    def new_base(self, base) :
        self.base = base
    def new_top(self, top) :
        self.top = top
    def printProc(self) :
        print(self.t0, self.tf, self.b, self.name, self.times)

def parse_command(command, sim_parameters) :
    run = 0
    if (command.split()[0] == "carrega") :
        sim_parameters[0] = command.split()[1]
    elif (command.split()[0] == "espaco") :
        sim_parameters[1] = int(command.split()[1])
    elif (command.split()[0] == "substitui") :
        sim_parameters[2] = int(command.split()[1])
    elif (command.split()[0] == "executa") :
        sim_parameters[3] = int(command.split()[1])
        run = 1
    elif (command.split()[0] == "sair") :
        sys.exit()
    else :
        print("Comando invalido!")
    return run

def arrive(to_arrive, l_procs, time, v_mem, alg, s, p) :
    for i in to_arrive :
        if (i.t0 == time) :
            l_procs.append(i)
            if (alg == 1) :
                best_fit(v_mem, i, s, p)
            elif (alg == 2) :
                worst_fit(v_mem, i, s, p)
    to_arrive[:] = [x for x in to_arrive if not x.t0 == time]
    return

def remove_procs(l_procs, time, v_mem, p_mem, indexes, matrix, algo) :
    for i in l_procs :
        #se for == da um bug as vezes !!!
        if (i.tf <= time) :
            print("Tempo: ", time, "| Removeu processo: ", i.pid)
            for j in range(len(v_mem)) :
                if (v_mem[j][0] == i.pid) :
                    v_mem[j][0] = -1

            for j in range(len(p_mem)) :
                if (p_mem[j] >= i.base and p_mem[j] < i.top):
                    p_mem[j] = -1
                    if (algo == 2) :
                        indexes.remove(j)
                    if (algo == 3) :
                        for k in range(len(p_mem)) :
                            matrix[j][k] = 0
            l_procs.remove(i)
    glue_mem(v_mem)

    return

def read_file(sim_parameters, to_arrive, compact) :
    pid = 0
    file = open(sim_parameters[0], "r")
    line = file.readline()
    tot, vit, s, p = int(line.split()[0]), int(line.split()[1]), int(line.split()[2]), int(line.split()[3])
    for line in file :
        if (len(line.split()) == 2) :
            compact.append(int(line.split()[0]))
            continue
        to_arrive.append(process(int(line.split()[0]),int(line.split()[1]),int(line.split()[2]),line.split()[3], pid))
        pid += 1
        for i in range(4, len(line.split()), 2) :
            to_arrive[-1].addExec(int(line.split()[i]), int(line.split()[i + 1]))
    file.close()
    return(tot, vit, s, p)

def glue_mem(v_mem) :
    #rejuntar a mémoria != desfragmentar
    i = 0
    while (i < len(v_mem)) :
        if (v_mem[i][0] != -1) :
            i += 1
            continue
        j = i
        #achar a ultima posição consecutiva que é igual i
        while (j < len(v_mem) and v_mem[i][0] == v_mem[j][0]) :
            j += 1
        j -= 1
        #atualizar a posição final de i
        v_mem[i][2] = v_mem[j][2]
        #retirar os elementos imcorporados em i
        while (j > i) :
            v_mem.pop(j)
            j -= 1
        i += 1

def page_fault(pos, proc, p, p_mem) :
    vit_pos = pos + proc.base*p
    vit_page = math.floor(vit_pos/p)
    if (vit_page in p_mem):
        return 0
    return 1

def FIFO(p_mem, indexes, proc, pos, p) :
    page = math.floor((pos + proc.base*p)/p)
    if (-1 not in p_mem) :
        i = indexes.pop(0)
        p_mem[i] = page
        indexes.append(i)
    else :
        i = p_mem.index(-1)
        p_mem[i] = page
        indexes.append(i)

def LRU2(p_mem, matrix, proc, pos, p, p_fault) :
    #Se deu page fault
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
    #Se não deu, atualiza a matrix
    else :
        least[0] = p_mem.index(page)
    for j in range(len(matrix[least[0]])) :
        matrix[least[0]][j] = 1
    for j in range(len(matrix[least[0]])) :
        matrix[j][least[0]] = 0

def best_fit(v_mem, proc, s, p) :
    #começar com um best que não existe
    best = -1
    #acha o número de paginas
    n_pages = math.ceil((math.ceil((proc.b)/s)*s)/p)
    for i in range(len(v_mem)) :
        #procura uma posição sem ninguém
        if (v_mem[i][0] == -1) :
            #ve se tem espaço suficiente
            if ((v_mem[i][2] - v_mem[i][1]) >= n_pages) :
                if (best == -1) :
                    best = i
                #ve se precisa mudar o best
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
    return

def worst_fit(v_mem, proc, s, p) :
    #começar com um best que não existe
    worst = -1
    #acha o número de paginas
    n_pages = math.ceil((math.ceil((proc.b)/s)*s)/p)
    for i in range(len(v_mem)) :
        #procura uma posição sem ninguém
        if (v_mem[i][0] == -1) :
            #ve se tem espaço suficiente
            if ((v_mem[i][2] - v_mem[i][1]) >= n_pages) :
                if (worst == -1) :
                    worst = i
                #ve se precisa mudar o worst
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

def switch_mem_pos(mem, i) :
    if (i + 1 == len(mem)) :
        return
    mem[i][0], mem[i + 1][0] = mem[i+ 1][0], mem[i][0]
    mem[i][2] = mem[i + 1][2] - mem[i + 1][1] + mem[i][1]
    mem[i + 1][1] = mem[i][2]
    return

def compact_vmem(mem) :
    i = 0
    while (i < len(mem) and mem[i][0] != -1):
        i += 1
    # Memória toda foi compactada
    if (i < len(mem)):
        switch_mem_pos(mem, i)
        glue_mem(mem)
        compact_vmem(mem[i + 1:])
    glue_mem(mem)
    return

def compact_pmem(mem, indexes, matrix, algo, size) :
    i = 0
    while (i < len(mem) and mem[i] != -1) :
        i += 1

    if (i < len(mem)) :
        j = i
        while (j < len(mem) and mem[j] == -1) :
            j += 1
        if (j < len(mem)) :
            mem[i] = mem[j]
            mem[j] = -1

            if (algo == 2) :
                l = indexes.index(j + size)
                indexes[l] = i + size
            if (algo == 3) :
                matrix[i + size] = [l for l in matrix[j + size]]
                matrix[j + size] = [0 for l in matrix[j + size]]
            mem[i+1:] = compact_pmem(mem[i+1:], indexes, matrix, algo, i+1)
    return mem

def fix_B_T(v_mem, l_procs, p_mem) :
    for i in l_procs :
        for j in v_mem :
            if (j[0] == i.pid) :
                for k in range(len(p_mem)) :
                    if (i.base <= p_mem[k] and p_mem[k] < i.top) :
                        p_mem[k] -= i.base - j[1]
                i.base, i.top = j[1], j[2]
                break

def simulation(sim_parameters) :
    to_arrive = []
    compact = []
    l_procs = []
    p_mem = []
    p_mem_indexes = []
    matrix = []
    time = 0
    tot, vit, s, p = read_file(sim_parameters, to_arrive, compact)
    v_mem = [[-1, 0, vit//p]]

    #agora criar os arquivos de memória
    for i in range(tot//p) :
        p_mem.append(-1)
    for i in range(len(p_mem)) :
        matrix.append([])
        for j in range(len(p_mem)) :
            matrix[i].append(0)

    #começa o loop do processo
    while (to_arrive or l_procs) :
        #colocar os caras da to_arrive na l_procs
        arrive(to_arrive, l_procs, time, v_mem, sim_parameters[1], s, p)
        for i in l_procs :
            while (i.times and i.times[0][1] == time) :

                if (page_fault(i.times[0][0], i, p, p_mem)) :
                    print("deu page_fault")
                    if (sim_parameters[2] == 2) :
                        FIFO(p_mem, p_mem_indexes, i, i.times[0][0], p)
                    elif (sim_parameters[2] == 3) :
                        LRU2(p_mem, matrix, i, i.times[0][0], p, 1)
                else :
                    if (sim_parameters[2] == 3) :
                        LRU2(p_mem, matrix, i, i.times[0][0], p, 0)
                    print("não deu page fault")
                i.times.pop(0)
        remove_procs(l_procs, time, v_mem, p_mem, p_mem_indexes, matrix, sim_parameters[2])
        if (compact and compact[0] == time) :
            compact_vmem(v_mem)
            compact_pmem(p_mem, p_mem_indexes , matrix, sim_parameters[2], 0)
            fix_B_T(v_mem, l_procs, p_mem)
            compact.pop(0)
        time += 1
        print("VIRTUAL ", v_mem)
        print("FÍSICA " , p_mem)
        if(sim_parameters[2] == 2) :
            print("INDICES ", p_mem_indexes)
        if(sim_parameters[2] == 3) :
            for i in range(len(p_mem)) :
                print(matrix[i])
        print("=========================================================================")
    return

def main() :
    sim_parameters = ["bob.txt", 1, 3, 1]
    while(True) :
        print("[ep3] :", end = "")
        command = input()
        if(parse_command(command, sim_parameters)) :
            simulation(sim_parameters)

main()
