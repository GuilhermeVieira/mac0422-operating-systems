# Guilherme Costa Vieira               Nº USP: 9790930
# Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
import os
# Escreve em binário o vetor vector no arquivo filename
def writeBin(filename, vector):
    vectorBytes = []

    for i in vector:
        vectorBytes.append(i.to_bytes(1, byteorder = 'big', signed = True))
    filepath = os.path.join('/tmp', filename)
    binFile = open(filepath, "wb")

    for i in vectorBytes:
        binFile.write(i)

    binFile.close()

# Retorna os bytes convertidos em inteiro lidos do arquivo filename.
def readBin(filename):
    filepath = os.path.join('/tmp', filename)
    binFile = open(filepath, "rb")
    bytesRead = []

    try:
        byte = binFile.read(1)
        while len(byte) > 0:
            i = int.from_bytes(byte, byteorder = 'big', signed = True)
            bytesRead.append(i)
            byte = binFile.read(1)
    finally:
        binFile.close()

    return bytesRead
