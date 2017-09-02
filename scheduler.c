#include <stdio.h>
#include <stdlib.h> //atoi
#include <string.h> //strcmp

int main(int argc, char **argv)
{
    int scheduler_type = atoi(argv[1]), optional = 0;
    char *file_input = argv[2], *file_output = argv[3];
    if(argc >= 4 && !strcmp("d", argv[4]))
        optional = 1;

    return 0;
}
