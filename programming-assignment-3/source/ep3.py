# Guilherme Costa Vieira               Nº USP: 9790930
# Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
import sys
import math
import os
from page import *
from fit import *
from mem import *
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
    #metodo usado para debug
    def printProc(self) :
        print(self.t0, self.tf, self.b, self.name, self.times)

#Processa o comando dado como input.
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

#Transfere todos os processos que tem t0 == time da to_arrive para a l_procs.
def arrive(to_arrive, l_procs, time, v_mem, alg, s, p, qf_sizes, available, p_mem) :
    for i in to_arrive :
        if (i.t0 == time) :
            l_procs.append(i)
            #Como os processos já chegaram vamos usar os alg's de fit para alocar
            #os espaço nescessário na memória virtual.
            if (alg == 1) :
                best_fit(v_mem, i, s, p)
            elif (alg == 2) :
                worst_fit(v_mem, i, s, p)
            else :
                quick_fit(v_mem, i, s, p, qf_sizes, available)
            print_mem(0, s, p, v_mem, p_mem, l_procs)
        #to_arrive esta ordenada, logo não precisamos continuar procurando.
        elif (i.t0 > time) :
            break
    #Tiramos os processos com t0 == time da to_arrive.
    to_arrive[:] = [x for x in to_arrive if not x.t0 == time]
    return

#Remove da l_procs todos os processos com tf == time e tira eles da v_mem, p_mem,
#e a estrutura adicional usada pelos alg's de page_fault.
def remove_procs(l_procs, time, v_mem, p_mem, indexes, matrix, count, algo) :
    to_remove = []
    for i in l_procs :
        if (i.tf == time) :
            #Retirar o processo da memória virtual.
            for j in range(len(v_mem)) :
                if (v_mem[j][0] == i.pid) :
                    v_mem[j][0] = -1
                    #Todo processo esta em uma parte continua da v_mem.
                    break
            for j in range(len(p_mem)) :
                #Retirar o processe da memória física.
                if (p_mem[j] >= i.base and p_mem[j] < i.top):
                    p_mem[j] = -1
                    #Remover a página das estruturas adicionais usadas pelos alg's
                    #de page_fault
                    if (algo == 2) :
                        indexes.remove(j)
                    elif (algo == 3) :
                        for k in range(len(p_mem)) :
                            matrix[j][k] = 0
                    else :
                        for k in range(len(p_mem)) :
                            count[j][k] = 0
            to_remove.append(i)
    for j in to_remove :
        l_procs.remove(j)
    glue_mem(v_mem)
    return

#Transfere do arquivo de trace para a to_arrive e compact. Cria a next_pages se
#o algoritmo OPTIMAL for usado.
def read_file(sim_parameters, to_arrive, compact, next_pages) :
    pid = 0
    #Path relativo.
    try:
        file = open(sim_parameters[0], "r")
    #Path absoluto.
    except OSError:
        filepath = os.path.join(sim_parameters[0], "")
        file = open(filepath, "r")
    line = file.readline()
    parse_line = line.split()
    tot, vit, s, p = int(parse_line[0]), int(parse_line[1]), int(parse_line[2]), int(parse_line[3])
    for line in file :
        parse_line = line.split()
        if (len(parse_line) == 2) :
            compact.append(int(parse_line[0]))
            continue
        to_arrive.append(process(int(parse_line[0]),int(parse_line[1]),int(parse_line[2]),parse_line[3], pid))
        pid += 1
        for i in range(4, len(parse_line), 2) :
            b = int(parse_line[i])
            t = int(parse_line[i + 1])
            to_arrive[-1].addExec(b, t)
            next_pages.append([to_arrive[-1], b, t])
    file.close()
    #Criação da estrutura adicional.

    #ordenar por t0.
    next_pages.sort(key = lambda x: x[2])
    pid += 1
    num_of_lists = math.ceil(pid*.1)
    sizes = []
    for i in to_arrive:
        x = math.ceil((math.ceil((i.b)/s)*s)/p)
        sizes.append(x)
    sizes = sorted(sizes, key = sizes.count, reverse = True)
    #Tirar os repetidos da seizes.
    for i in sizes:
        x = sizes.count(i)
        if (x > 1):
            for j in range (x - 1):
                sizes.remove(i)
    sizes = sizes[:num_of_lists]
    return(tot, vit, s, p, sizes)

def simulation(sim_parameters) :
    to_arrive = []
    l_procs = []
    compact = []
    p_mem_indexes = []
    matrix = []
    count = []
    next_pages = []
    time = 0
    tot, vit, s, p, qf_sizes = read_file(sim_parameters, to_arrive, compact, next_pages)
    v_mem = [[-1, 0, math.ceil(vit/p)]]
    p_mem = [-1 for i in range(math.ceil(tot/p))]
    #Criação das estruturas adicionais de cada algoritmo.
    available = find_available(v_mem, qf_sizes)
    for i in range(len(p_mem)) :
        matrix.append([])
        count.append([])
        for j in range(len(p_mem)) :
            matrix[i].append(0)
            count[i].append(0)
    print_mem(0, s, p, v_mem, p_mem, l_procs)

    #começa o loop dos processos.
    while (to_arrive or l_procs) :
        #Coloca todos os processos que chegaram na l_procs.
        arrive(to_arrive, l_procs, time, v_mem, sim_parameters[1], s, p, qf_sizes, available, p_mem)
        for i in l_procs :
            #Carrega as páginas nescessárias.
            while (i.times and i.times[0][1] == time) :
                if (sim_parameters[2] == 1) :
                    next_pages.pop(0)
                if (page_fault(i.times[0][0], i, p, p_mem)) :
                    if (sim_parameters[2] == 1) :
                        OPTIMAL(p_mem, next_pages, i, i.times[0][0], p, time)
                    elif (sim_parameters[2] == 2) :
                        FIFO(p_mem, p_mem_indexes, i, i.times[0][0], p)
                    elif (sim_parameters[2] == 3) :
                        LRU2(p_mem, matrix, i, i.times[0][0], p, 1)
                    else :
                        LRU4(p_mem, count, i, i.times[0][0], p, 1)
                #Atuaixa as estruturas mesmo se não houve page_fault.
                else :
                    if (sim_parameters[2] == 3) :
                        LRU2(p_mem, matrix, i, i.times[0][0], p, 0)
                    else :
                        LRU4(p_mem, count, i, i.times[0][0], p, 0)
                print_mem(0, s, p, v_mem, p_mem, l_procs)
                i.times.pop(0)
        remove_procs(l_procs, time, v_mem, p_mem, p_mem_indexes, matrix, count, sim_parameters[2])
        #Atualiza os arquivos de memória depois de remover processos.
        print_mem(0, s, p, v_mem, p_mem, l_procs)
        if (compact and compact[0] == time) :
            compact_vmem(v_mem)
            compact_pmem(p_mem, p_mem_indexes , matrix, count, sim_parameters[2], 0)
            fix_B_T(v_mem, l_procs, p_mem)
            compact.pop(0)
            print_mem(0, s, p, v_mem, p_mem, l_procs)
        #Refaz a available depois de remover processos e compactar a memória
        if (sim_parameters[1] == 3)  :
            available = find_available(v_mem, qf_sizes)
        #atualizar o bit R
        if (sim_parameters[2] == 4) :
            for i in range(len(count)):
                count[i][0] = 0
        if (time%sim_parameters[3] == 0 and time != 0) :
            print_mem(1, s, p, v_mem, p_mem, l_procs)
        time += 1
    return

def main() :
    sim_parameters = ["bob.txt", 1, 1, 1]
    while(True) :
        print("[ep3] :", end = "")
        command = input()
        if(parse_command(command, sim_parameters)) :
            simulation(sim_parameters)

main()
