from random import uniform
from subprocess import call
from xlwt import Workbook

def generateProcesses(name, n_files_generated, n, maxt0, maxdt):
	mint0 = 0
	mindt = 0.1
	max_deadline = maxdt*n

	for j in range(n_files_generated):
		t0_vector = []
		f = open(str(name)+str(j+1)+".txt",'w')

		# Gera vetor de t0
		for i in range(n):
			t0_vector.append(round(uniform(mint0, maxt0),1))
		t0_vector.sort()

		for i in range(n):
			dt = round(uniform(mindt, maxdt), 1)
			deadline = round(uniform(t0_vector[i] + dt, t0_vector[i] + dt*5), 1)
			f.write(str(t0_vector[i]) + " " + str(dt) + " " + str(deadline) + " " + "processo" + str(i+1)+"\n")
		f.close()

	return

def main():
	input_name = "input" # Nome do arquivo de input
	output_name = "output"
	n_files_generated = 30 # Número de arquivos gerados
	n_processes = 64 # Número de processos.
	maxt0 = 120 # Limite de t0 para 64
	n_processes = 32 # Número de processos.
	maxt0 = 45 # Limite de t0 para 64
	n_processes = 8 # Número de processos.
	maxt0 = 15 # Limite de t0 para 64
	maxdt = 5 # Limite de dt

	generateProcesses(input_name, n_files_generated, n_processes, maxt0, maxdt)

	# Rodar para o SJF
	for i in range(n_files_generated):
		call(["time", "./ep1", "1", str(input_name) + str(i+1) + ".txt", str(output_name) + "SJF" + str(i+1) + ".txt"])

	# Rodar para o Round Robin
	for i in range(n_files_generated):
		call(["time", "./ep1", "2", str(input_name) + str(i+1) + ".txt", str(output_name) + "RR" + str(i+1) + ".txt"])

	# Rodar para o Priority
	#for i in range(n_files_generated):
		call(["time", "./ep1", "3", str(input_name) + str(i+1) + ".txt", str(output_name) + "PRR" + str(i+1) + ".txt"])

	deadlines = [] # Matriz das deadlines

	contexto_SJF = []
	contexto_RR = []
	contexto_PRR =[]

	# Pega as deadlines.
	for i in range(n_files_generated):
		f = open(str(input_name) + str(i+1) + ".txt", "r")
		deadlines.append([])
		for j in range(n_processes):
			line = f.readline()
			line_parsed = line.split(" ")
			deadlines[i].append(float(line_parsed[2]))
		f.close()

	# Configura a sheet das deadlines
	wb = Workbook()
	deadlines_sheet = wb.add_sheet('Deadlines')
	deadlines_sheet.write(0,0, "MEDIDAS")
	for i in range(n_files_generated):
		deadlines_sheet.write(i+1,0, "Medição "+str(i+1))
	deadlines_sheet.write(0,1, "SJF")
	deadlines_sheet.write(0,2,"RR")
	deadlines_sheet.write(0,3,"PRR")

	line = []
	line_parsed = []

	# Escreve se o SJF cumpriu as deadlines
	for i in range(n_files_generated):
		deadline_counter = 0
		f = open(str(output_name) + "SJF" + str(i+1) + ".txt", "r")
		for j in range(n_processes):
			line = f.readline()
			line_parsed = line.split(" ")
			if (deadlines[i][j] - float(line_parsed[1]) >= 0): #instante em que o processo parou
				deadline_counter += 1
		deadlines_sheet.write(i+1,1, deadline_counter) # Nº que cumpriu deadline
		f.close()

	line = []
	line_parsed = []

	# Escreve se o RR cumpriu as deadlines
	for i in range(n_files_generated):
		deadline_counter = 0
		f = open(str(output_name) + "RR" + str(i+1) + ".txt", "r")
		for j in range(n_processes):
			line = f.readline()
			line_parsed = line.split(" ")
			if (deadlines[i][j] - float(line_parsed[1]) >= 0): #instante em que o processo parou
				deadline_counter += 1
		deadlines_sheet.write(i+1,2, deadline_counter) # Nº que cumpriu deadline
		f.close()

	line = []
	line_parsed = []

	# Escreve se o PRR cumpriu as deadlines
	for i in range(n_files_generated):
		deadline_counter = 0
		f = open(str(output_name) + "PRR" + str(i+1) + ".txt", "r")
		for j in range(n_processes):
			line = f.readline()
			line_parsed = line.split(" ")
			if (deadlines[i][j] - float(line_parsed[1]) >= 0): #instante em que o processo parou
				deadline_counter += 1
		deadlines_sheet.write(i+1,3, deadline_counter) # Nº que cumpriu deadline
		f.close()

	# Configura a sheet do context switch
	cs_sheet = wb.add_sheet('Context Switch')
	for i in range(n_files_generated):
		cs_sheet.write(i+1,0, "Medição "+str(i+1))
	cs_sheet.write(0,1, "SJF")
	cs_sheet.write(0,2,"RR")
	cs_sheet.write(0,3,"PRR")

	line = []

	# Escreve o total de mudanças de contexto do SJF
	for i in range(n_files_generated):
		cs_counter = 0
		f = open(str(output_name) + "SJF" + str(i+1) + ".txt", "r")
		for j in range(n_processes):
			line = f.readline()
		line = f.readline() #Lê a linha com o total de mudanças de contexto
		cs_sheet.write(i+1,1, int(line)) # Nº de mudança de contexto
		f.close()

	line = []

	# Escreve o total de mudanças de contexto do RR
	for i in range(n_files_generated):
		cs_counter = 0
		f = open(str(output_name) + "RR" + str(i+1) + ".txt", "r")
		for j in range(n_processes):
			line = f.readline()
		line = f.readline() #Lê a linha com o total de mudanças de contexto
		cs_sheet.write(i+1,2, int(line)) # Nº de mudança de contexto
		f.close()

	line = []

	# Escreve o total de mudanças de contexto do PRR
	for i in range(n_files_generated):
		cs_counter = 0
		f = open(str(output_name) + "PRR" + str(i+1) + ".txt", "r")
		for j in range(n_processes):
			line = f.readline()

		line = f.readline() #Lê a linha com o total de mudanças de contexto
		cs_sheet.write(i+1,3, int(line)) # Nº de mudança de contexto
		f.close()

	wb.save('acabeiDeRodarRENOMEAR.xls')

main()
