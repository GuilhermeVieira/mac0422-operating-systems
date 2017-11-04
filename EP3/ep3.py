import sys
import math

class process:
    def __init__(self, t0, tf, b, name) :
        self.t0 = t0
        self.tf = tf
        self.b = b
        self.name = name
        self.times = []
    def addExec(self, tx, px) :
        self.times.append((px, tx))
    def new_base(self, base) :
        self.base = base
    def new_top(self, top) :
        self.top = top
    def printProc(self) :
        print(self.t0, self.tf, self.b, self.name, self.times)

def analyseCommand(command, sim_parameters) :
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
    to_arrive[:] = [x for x in to_arrive if not x.t0 == time]
    return

def remove_procs(l_procs, time, v_mem) :
    for i in l_procs :
        if (i.tf == time) :
            j = 0
            for j in range(len(v_mem)) :
                if (v_mem[j][1] <= i.base and i.top <= v_mem[j][2]) :
                    if (v_mem[j][2] > i.top) :
                        v_mem.insert(j + 1, [0, i.top, v_mem[j][2]])
                    if (v_mem[j][1] < i.base) :
                        v_mem[j][2] = i.base
                        v_mem.inser(j + 1, [-1, i.base, i.top])
                    else :
                        v_mem[j][0], v_mem[j][2] = -1, i.top
            l_procs.remove(i)
    #colocar em função ???        
    #rejuntar a mémoria != desfragmentar
    i = 0
    while (i < len(v_mem)) :
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

def read_file(sim_parameters, to_arrive, compact) :
    file = open(sim_parameters[0], "r")
    line = file.readline()
    tot, vit, s, p = int(line.split()[0]), int(line.split()[1]), int(line.split()[2]), int(line.split()[3])
    for line in file :
        if (len(line.split()) == 2) :
            compact.append(line.split()[0])
            continue
        to_arrive.append(process(int(line.split()[0]),int(line.split()[1]),int(line.split()[2]),line.split()[3]))
        for i in range(4, len(line.split()), 2) :
            to_arrive[-1].addExec(int(line.split()[i]), int(line.split()[i + 1]))
    file.close()
    return(tot, vit, s, p)

def best_fit(v_mem, proc, s, p) :
    #começar com um best que não existe
    best = -1
    #acha o número de paginas
    n_pages = (math.ceil((proc.b)/s)*s)/p
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
    if (v_mem[best][2] - v_mem[best][1] > n_pages) :
        v_mem.insert(best + 1 ,[-1, v_mem[best][1] + n_pages, v_mem[best][2]])
    v_mem[best][0], v_mem[best][2] = 0, v_mem[best][1] + n_pages
    proc.new_base(v_mem[best][1])
    proc.new_top(v_mem[best][2])
    #rejuntar a mémoria != desfragmentar
    i = 0
    while (i < len(v_mem)) :
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
    return

def simulation(sim_parameters) :
    to_arrive = []
    compact = []
    l_procs = []
    p_mem = []
    time = 0
    tot, vit, s, p = read_file(sim_parameters, to_arrive, compact)
    v_mem = [[-1, 0, vit//p]]
    #agora criar os arquivos de memória
    for i in range (tot//p) :
        p_mem.append(-1)
    #começa o loop do processo
    while (to_arrive or l_procs) :
        #colocar os caras da to_arrive na l_procs
        arrive(to_arrive, l_procs, time, v_mem, sim_parameters[1], s, p)
        #percorrer a l_procs para executar os algs
        remove_procs(l_procs, time, v_mem)
        time += 1
        print(v_mem)
    return

def main() :
    sim_parameters = ["", 1, 1, 1]
    while(True) :
        print("[ep3] :", end = "")
        command = input()
        if(analyseCommand(command, sim_parameters)) :
            simulation(sim_parameters)
    return

main()
