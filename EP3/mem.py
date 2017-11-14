from binary import *

#Junta todas as partes vazias concecutivas da memória.
def glue_mem(v_mem) :
    #rejuntar a mémoria != desfragmentar
    i = 0
    while (i < len(v_mem)) :
        if (v_mem[i][0] != -1) :
            i += 1
            continue
        j = i
        #achar a ultima posição consecutiva com -1.
        while (j < len(v_mem) and v_mem[i][0] == v_mem[j][0]) :
            j += 1
        j -= 1
        #atualizar a posição final de i
        v_mem[i][2] = v_mem[j][2]
        #retirar os elementos incorporados em i
        while (j > i) :
            v_mem.pop(j)
            j -= 1
        i += 1

#Troca o conteudo de mem[i] com mem[-1], atualizando as bases e os topos.
def switch_mem_pos(mem, i) :
    if (i + 1 == len(mem)) :
        return
    mem[i][0], mem[i + 1][0] = mem[i+ 1][0], mem[i][0]
    mem[i][2] = mem[i + 1][2] - mem[i + 1][1] + mem[i][1]
    mem[i + 1][1] = mem[i][2]
    return

#Compacta a memória virtual.
def compact_vmem(mem) :
    i = 0
    while (i < len(mem) and mem[i][0] != -1):
        i += 1
    if (i < len(mem)):
        switch_mem_pos(mem, i)
        glue_mem(mem)
        compact_vmem(mem[i + 1:])
    glue_mem(mem)
    return

#Compacta a memória real e muda as estruturas adicionais.
def compact_pmem(mem, indexes, matrix, count, algo, size) :
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
            elif (algo == 3) :
                matrix[i + size] = [l for l in matrix[j + size]]
                matrix[j + size] = [0 for l in matrix[j + size]]
            elif (algo == 4) :
                count[i + size] = [l for l in count[j + size]]
                count[j + size] = [0 for l in count[j + size]]
            mem[i+1:] = compact_pmem(mem[i+1:], indexes, matrix, count, algo, i+1)
    return mem

#Se houve uma compactação a base e o topo de um processo deve ser alterado.
def fix_B_T(v_mem, l_procs, p_mem) :
    for i in l_procs :
        for j in v_mem :
            if (j[0] == i.pid) :
                for k in range(len(p_mem)) :
                    if (i.base <= p_mem[k] and p_mem[k] < i.top) :
                        p_mem[k] -= i.base - j[1]
                i.base, i.top = j[1], j[2]
                break

def printMem(p, v_mem, p_mem) :
    print("Virtual: ")
    print(v_mem)
    
    MemState =[]
    for i in v_mem :
        for j in range(i[2] - i[1]) :
            for k in range(p) :
                #print(repr(i[0]).rjust(3), end = "")
                MemState.append(i[0])
    
    writeBin("ep3.vir", MemState)
    print(readBin("ep3.vir"))
    print("\n")

    MemState =[]

    print("Física:")
    print(p_mem)
    
    for i in p_mem:
        for k in range(p) :
            #print(repr(i).rjust(3), end = "")
            MemState.append(i)

    writeBin("ep3.mem", MemState)
    print(readBin("ep3.mem"))
    print("\n")
    
