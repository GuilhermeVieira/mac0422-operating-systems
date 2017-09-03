#include <string.h> //strcmp()
#include <stdlib.h> //atoi()
#include "fileHandlerr.h"

int main(int argc, char **argv)
{
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *inputFile, *outputFile;
    List bob;
    inputFile = argv[2];
    outputFile = argv[3];
    if (argc >= 4 && strcmp("d", argv[4])){
        optional = 1;
    }
    bob = readFile(inputFile);
    return 0;
}
