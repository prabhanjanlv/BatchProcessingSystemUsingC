Process.c

This program will be launched in fork_execv (see fork_execv.c)
Compilation Instruction: 
gcc process.c -o process
 
Reference: https://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1044654269&id=1043284392

batchsample.c

This sample source code demonstrates the development of 
a batch-job scheduler using pthread.

Compilation Instruction: 
gcc pthread_sample.c -o pthread_sample -lpthread

Learning Objectives:
1. To compile and run a program powered by the pthread library
2. To create two concurrent threads: a scheduling thread and a dispatching thread 
3. To execute jobs in the AUbatch system by the dispatching thread
4. To synchronize the two concurrent threads using condition variables

How to run aubatch_sample?
1. You need to compile another sample code: process.c
2. The "process" program (see process.c) takes two input arguments
from the commandline
3. In aubtach: type ./process 5 10 to submit program "process" as a job.
 
Compilation Instruction:
gcc commandline_parser.c -o commandline_parser
./commandline_parser