# Escreve em binário o vetor vector no arquivo filename
def writeBin(filename, vector):
    vectorBytes = []

    for i in vector:
        vectorBytes.append(i.to_bytes(1, byteorder='big', signed=True))
    
    binFile = open(filename, "wb")
    
    for i in vectorBytes:
        binFile.write(i)

    binFile.close()

# Retorna os bytes convertidos em inteiro lidos do arquivo filename.
def readBin(filename):
    binFile = open(filename,"rb")
    bytesRead = []

    try:
        byte = binFile.read(1)
        while len(byte) > 0:
            i = int.from_bytes(byte, byteorder='big', signed=True)
            bytesRead.append(i)
            byte = binFile.read(1)
    finally:
        binFile.close()

    return bytesRead
