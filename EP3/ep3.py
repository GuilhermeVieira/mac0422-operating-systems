import sys

class process:
    def __init__(self, t0, tf, b, name) :
        self.t0 = t0
        self.tf = tf
        self.b = b
        self.name = name
        self.times = []
    def addExec(self, tx, px) :
        self.times.append((px, tx))
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

def simulation(sim_parameters) :
    coisas = []
    file = open(sim_parameters[0], "r")
    for line in file :
        coisas.append(process(line.split()[0],line.split()[1],line.split()[2],line.split()[3]))
        for i in range(4, len(line.split()), 2) :
            coisas[-1].addExec(int(line.split()[i]), int(line.split()[i + 1]))
    for i in coisas :
        i.printProc()

def main() :
    sim_parameters = ["", 0, 0, 1]
    while(True) :
        print("[ep3] :", end = "")
        command = input()
        if(analyseCommand(command, sim_parameters)) :
            simulation(sim_parameters)

main()
