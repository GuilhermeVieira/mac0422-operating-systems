import sys
#substituir por um objeto seria legal
def analyseCommand(command):
    run = 0
    if (command.split()[0] == "carrega") :
        global file_name
        file_name = command.split()[1]
    elif (command.split()[0] == "espaco") :
        global mem_alg
        mem_alg = command.split()[1]
    elif (command.split()[0] == "substitui") :
        global page_alg
        page_alg = command.split()[1]
    elif (command.split()[0] == "executa") :
        global del_Time
        del_Time = command.split()[1]
        run = 1
    elif (command.split()[0] == "sair") :
        sys.exit()
    else :
        print("Comando invalido!")
    return run

def simulation() :
    print("irei fazer coisas")

def main() :
    while(True) :
        print("[ep3] :", end = "")
        command = input()
        if(analyseCommand(command)) :
            simulation()

main()
