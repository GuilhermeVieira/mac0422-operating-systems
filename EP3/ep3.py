import sys

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

def simulation(sim_parameters) :
    print(sim_parameters)

def main() :
    sim_parameters = ["", 0, 0, 1]
    while(True) :
        print("[ep3] :", end = "")
        command = input()
        if(analyseCommand(command, sim_parameters)) :
            simulation(sim_parameters)

main()
