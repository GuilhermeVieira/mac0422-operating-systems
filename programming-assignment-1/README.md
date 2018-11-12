### Programming Assignment 1

---

Guilherme Costa Vieira               Nº USP: 9790930

Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

---

The python script was made to run the program several times to make statistical experiments for the report.

To compile the binaries run the following command on the terminal:

`make
`

If you only need to compile the shell, run:

`make ep1sh`

If you only need to compile the CPU schedulers, run:

`make ep1`

To remove the .o files created in the compilation process, run:

`make clean`

The following command runs the shell:

`./ep1sh`

If you need root permissions, run:

`sudo ./ep1sh`

Run the CPU schedulers with the following command:

`./ep1 <arg1> <arg2> <arg3> <optionalArg4>`

where: `<arg>` is an integer from 1 to 3 that represents which scheduler algorithm is going to be used (1 = Shortest Job First, 2 = Round Robin, 3 = Priority scheduler), `<arg2>` is the scheduler input file name (.txt format or similar), `<arg3>` is the scheduler output file name (.txt format or similar) and `<optionalArg4>` is the extra parameter, if used with the letter `d`, it will display on the stderr stream: the processes arrival in the system, informing the trace line of that process; the processes CPU usage, informing which process started using the CPU and which CPU it is using; a process leaving the CPU, informing which process left the CPU and which CPU is now free; the ending of process execution, informing which line it is being written in the output file and the quantity of context switches in the moment they happened.
