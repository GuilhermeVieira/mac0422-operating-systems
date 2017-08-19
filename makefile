ep1sh: ep1sh.c Buffer.c embdCmds.c
	gcc ep1sh.c Buffer.c embdCmds.c -Wall -g -lreadline -lexplain  -o ep1sh -I.
