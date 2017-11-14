from random import randint

def writeBin():
	vector = []
	for i in range(randint(2, 10)):
		vector.append(randint(-1, 126))

	print("Vetor gerado:")
	print(vector)
	print("\n")
	vectorBytes = []
	for i in vector:
		vectorBytes.append(i.to_bytes(1, byteorder='big', signed=True))
	# make file
	newFile = open("ep3teste.mem", "wb")
	# write to file
	for i in vectorBytes:
		newFile.write(i)

	newFile.close()

def readBin():
	file = open("ep3teste.mem","rb")
	bytesRead = []

	try:
	    byte = file.read(1)
	    while len(byte) > 0:
	        # Do stuff with byte.
	        i = int.from_bytes(byte, byteorder='big', signed=True)
	        bytesRead.append(i)
	        byte = file.read(1)
	finally:
	    file.close()

	print("Vetor lido:")
	print(bytesRead)
	print("\n")

def main():
	#print("HA")
	for i in range(10):
		writeBin()
		readBin()
		#print("Foi")
	#print("LOL")
main()