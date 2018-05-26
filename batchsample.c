#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

typedef unsigned int u_int; 

//#define LOW_ARRIVAL_RATE /* Long arrivel-time interval */
#define LOW_SERVICE_RATE   /* Long service time */

/* 
 * Static commands are submitted to the job queue.
 * When comment out the following macro, job are submitted by users.
 */
//#define STATIC_COMMAND 

#define CMD_BUF_SIZE 30 /* The size of the command queueu */
#define NUM_OF_CMD   5  /* The number of submitted jobs   */
#define MAX_CMD_LEN  512 /* The longest commandline length */

/* 
 * When a job is submitted, the job must be compiled before it
 * is running by the executor thread (see also executor()).
 */
void *commandline( void *ptr ); /* To simulate job submissions and scheduling */
void *executor( void *ptr );    /* To simulate job execution */

pthread_mutex_t cmd_queue_lock;  /* Lock for critical sections */
pthread_cond_t cmd_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t cmd_buf_not_empty; /* Condition variable for buf_not_empty */

/* Global shared variables */
u_int buf_head;
u_int buf_tail;
u_int count;
char *cmd_buffer[CMD_BUF_SIZE];

/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  4 
#define MAXCMDLINE   64 

int NJOBS=0;
char cpolicy[8]="fcfs";
char btimes[5];


struct Jobs{
	char jname[10];
	int btime;
	int priority;
	int order;
}Job[5],temp;

void menu_execute(char *line, int isargs); 
int cmd_run(int nargs, char **args); 
int cmd_quit(int nargs, char **args); 
void showmenu(const char *name, const char *x[]);
int cmd_helpmenu(int n, char **a);
int cmd_dispatch(char *cmd);
void sub_main();
void printstatement(char *args);

/*
 * The run command - submit a job.
 */
int cmd_run(int nargs, char **args) {
	if (nargs != 4) {
		printf("Usage: run <job> <time> <priority>\n");
		return EINVAL;
	}
		
		if(NJOBS >= sizeof Job / sizeof Job[0])
        return 0;
	
		
        /* Use execv to run the submitted job in AUbatch */
		
		strcpy(Job[NJOBS].jname, args[1]);
		Job[NJOBS].order=NJOBS+1;
		int bt = atoi(args[2]);
		int pri = atoi(args[3]);
		Job[NJOBS].btime=bt;
		Job[NJOBS].priority=pri;
		NJOBS++;
		printf("use execv to run the job in AUbatch.\n");
		sub_main();
		
      	return 1; /* if succeed */
}
 /* void printstatement(char *args) {
	 printf("this is a function:%s\n",args);
 } */
/*
 * The list command.
 */
 
int cmd_list(int nargs, char **args) {
	int i;
	printf("The Current Scheduling policy is:%s\n",cpolicy);
	printf("The following are the list of jobs:\n");
		printf("Order\tJob Name\tBurst Time\tPriority\n");
		
		for(i=0;i<NJOBS;i++)
		{
			printf("%d\t%s\t\t%d\t\t%d\n",Job[i].order,Job[i].jname,Job[i].btime,Job[i].priority);			
		}
        return(0);
} 
/*
 * The Swap command.
 */
 
int cmd_swap(int nargs, char **args) {	
	int spflag = 0;
	int i,j;
	char *p = args[1];
	p[strlen(p)-1] = 0;

	if(strcmp(args[1],"fcfs") == 0)
	spflag = 1;
	if(strcmp(args[1],"sjf") == 0)
	spflag = 1;
	if(strcmp(args[1],"priority") == 0)
	spflag = 1;
	
	if((nargs == 2)&&(spflag == 1))
	{
	strcpy(cpolicy, args[1]);
	printf("Jobs are now scheduled per %s\n",cpolicy);

	if(strcmp(args[1],"fcfs") == 0)
	{
	 for (i = 1; i < NJOBS; i++)
      for (j = 0; j < NJOBS - i; j++) {
         if (Job[j].order > Job[j + 1].order){
            temp = Job[j];
            Job[j] = Job[j + 1];
            Job[j + 1] = temp;
         }
      }	
	}
	if(strcmp(args[1],"sjf") == 0)
	{
	for (i = 1; i < NJOBS; i++)
      for (j = 0; j < NJOBS - i; j++) {
         if (Job[j].btime > Job[j + 1].btime){
            temp = Job[j];
            Job[j] = Job[j + 1];
            Job[j + 1] = temp;
         }
      }		
	}
	if(strcmp(args[1],"priority") == 0)
	{
	for (i = 1; i < NJOBS; i++)
      for (j = 0; j < NJOBS - i; j++) {
         if (Job[j].priority > Job[j + 1].priority){
            temp = Job[j];
            Job[j] = Job[j + 1];
            Job[j + 1] = temp;
         }
      }		
	}
        return(0);
	}	
	else
	{
		printf("Usage: swap <fcfs/sjf/priority>(pick one)\n");
		return EINVAL;		
	}
} 
/*
 * The quit command.
 */
int cmd_quit(int nargs, char **args) {
	printf("Please display performance information before exiting AUbatch!\n");
        exit(0);
}

/*
 * Display menu information
 */
void showmenu(const char *name, const char *x[])
{
	int ct, half, i;

	printf("\n");
	printf("%s\n", name);
	
	for (i=ct=0; x[i]; i++) {
		ct++;
	}
	half = (ct+1)/2;

	for (i=0; i<half; i++) {
		printf("    %-36s", x[i]);
		if (i+half < ct) {
			printf("%s", x[i+half]);
		}
		printf("\n");
	}

	printf("\n");
}

static const char *helpmenu[] = {
	"[run] <job> <time> <priority>       ",
	"[list] List all Job                 ",
	"[swap] Swap Scheduling policy   	 ",
	"[help] Print help menu              ",
	"[quit] Exit AUbatch                 ",
        /* Please add more menu options below */
	NULL
};

int cmd_helpmenu(int n, char **a)
{
	(void)n;
	(void)a;

	showmenu("AUbatch help menu", helpmenu);
	return 0;
}

/*
 *  Command table.
 */
static struct {
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	/* commands: single command must end with \n */
	{ "?\n",	cmd_helpmenu },
	{ "h\n",	cmd_helpmenu },
	{ "help\n",	cmd_helpmenu },
	{ "r",		cmd_run },
	{ "run",	cmd_run },
	{ "q\n",	cmd_quit },
	{ "quit\n",	cmd_quit },
	{ "list\n",	cmd_list },
	{ "l\n",	cmd_list },
	{ "swap",	cmd_swap },
	{ "s",	cmd_swap },
        /* Please add more operations below. */
        {NULL, NULL}
};

/*
 * Process a single command.
 */
int cmd_dispatch(char *cmd)
{
	//printf("Entered command is :%s\n",cmd);
	time_t beforesecs, aftersecs, secs;
	u_int32_t beforensecs, afternsecs, nsecs;
	char *args[MAXMENUARGS];
	int nargs=0;
	char *word;
	char *context;
 	int i, result;

	for (word = strtok_r(cmd, " ", &context);
	     word != NULL;
	     word = strtok_r(NULL, " ", &context)) {

		if (nargs >= MAXMENUARGS) {
			printf("Command line has too many words\n");
			return E2BIG;
		}
		args[nargs++] = word;
	}

	if (nargs==0) {
		return 0;
	}

	for (i=0; cmdtable[i].name; i++) {
		if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name)) {
			assert(cmdtable[i].func!=NULL);
            
            /*Qin: Call function through the cmd_table */
			result = cmdtable[i].func(nargs, args);
			return result;
		}
	}

	printf("%s: Command not found\n", args[0]);
	return EINVAL;
}

void sub_main() {
    pthread_t command_thread, executor_thread; /* Two concurrent threads */
    char *message1 = "Command Thread";
    char *message2 = "Executor Thread";
    int  iret1, iret2;

    /* Initilize count, two buffer pionters */
    count = 0; 
    buf_head = 0;  
    buf_tail = 0; 

    /* Create two independent threads:command and executors */
    iret1 = pthread_create(&command_thread, NULL, commandline, (void*) message1);
    iret2 = pthread_create(&executor_thread, NULL, executor, (void*) message2);

    /* Initialize the lock the two condition variables */
    pthread_mutex_init(&cmd_queue_lock, NULL);
    pthread_cond_init(&cmd_buf_not_full, NULL);
    pthread_cond_init(&cmd_buf_not_empty, NULL);
     
    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    pthread_join(command_thread, NULL);
    pthread_join(executor_thread, NULL); 

    printf("command_thread returns: %d\n",iret1);
    printf("executor_thread returns: %d\n",iret2);
    exit(0);
}

/*
 * Command line main loop.
 */

int main() {
	char *buffer;
    size_t bufsize = 64;
        
        buffer = (char*) malloc(bufsize * sizeof(char));
        if (buffer == NULL) {
 		perror("Unable to malloc buffer");
 		exit(1);
	}
 
     while (1) {
		printf("> [? for menu]: ");
		getline(&buffer, &bufsize, stdin);
		cmd_dispatch(buffer);
		printf("test:%s\n",buffer);
	} 
	sub_main();
	
}
/* 
 * This function simulates a terminal where users may 
 * submit jobs into a batch processing queue.
 * Note: The input parameter (i.e., *ptr) is optional. 
 * If you intend to create a thread from a function 
 * with input parameters, please follow this example.
 */
void *commandline(void *ptr) {
    char *message;
    char *temp_cmd;
    u_int i;
    char num_str[8];
    size_t command_size = 64;
     
    message = (char *) ptr;
   

    /* Enter multiple commands in the queue to be scheduled */
    for (i = 0; i < NUM_OF_CMD; i++) {    
        /* lock the shared command queue */
        pthread_mutex_lock(&cmd_queue_lock);
 
        printf("In commandline: count = %d\n", count);
        while (count == CMD_BUF_SIZE) {
            pthread_cond_wait(&cmd_buf_not_full, &cmd_queue_lock);
        }

        /* Dynamically create a buffer slot to hold a commandline */
#ifdef STATIC_COMMAND
        cmd_buffer[buf_head] = malloc(strlen("process -help -time ") + 1);
        strcpy(cmd_buffer[buf_head], "./process "); 
        sprintf(num_str, "%d", i);
        strcat(cmd_buffer[buf_head], message);
#else
        pthread_mutex_unlock(&cmd_queue_lock);
        printf("Please submit a batch processing job(eg:<./process>) or basic commands(eg: <list> <run> <swap>:\n");
        printf(">"); 
        temp_cmd = malloc(MAX_CMD_LEN*sizeof(char));
        getline(&temp_cmd, &command_size, stdin);
		if((strcmp(temp_cmd,"list")>0)||(strcmp(temp_cmd,"swap")>0)||(strcmp(temp_cmd,"help")>0)||(strcmp(temp_cmd,"run")>0)||(strcmp(temp_cmd,"quit")>0))
		cmd_dispatch(temp_cmd);
	else{
        pthread_mutex_lock(&cmd_queue_lock);    
        cmd_buffer[buf_head]= temp_cmd; 
	}
        
#endif
        printf("In commandline: cmd_buffer[%d] = %s\n", buf_head, cmd_buffer[buf_head]);     
        count++;
        /* Move buf_head forward, this is a circular queue */ 
        buf_head++;
        if (buf_head == CMD_BUF_SIZE)
            buf_head = 0;

        pthread_cond_signal(&cmd_buf_not_empty);  
        /* Unlok the shared command queue */
        pthread_mutex_unlock(&cmd_queue_lock);

        /* Simulate a low arrival rate */
#ifdef LOW_ARRIVAL_RATE
        sleep(2); /* Simulate an arrival time of 2 seconds */
#endif
    } /* end for */
}
/*
 * This function simulates a server running jobs in a batch mode.
 * Note: The input parameter (i.e., *ptr) is optional. 
 * If you intend to create a thread from a function 
 * with input parameters, please follow this example.
 */
void *executor(void *ptr) {
    char *message;
    u_int i;

    message = (char *) ptr;
    printf("%s \n", message);

    for (i = 0; i < NUM_OF_CMD; i++) {
        /* lock and unlock for the shared process queue */
        pthread_mutex_lock(&cmd_queue_lock);
        printf("In executor: count = %d\n", count);
        while (count == 0) {
            pthread_cond_wait(&cmd_buf_not_empty, &cmd_queue_lock);
        }

        /* Run the command scheduled in the queue */
        count--;
        printf("In executor: cmd_buffer[%d] = %s\n", buf_tail, cmd_buffer[buf_tail]); 
        
        /* 
         * Note: system() function is a simple example.
         * You should use execv() rather than system() here.
         */		 
        system(cmd_buffer[buf_tail]); 
        /* Free the dynamically allocated memory for the buffer */
        free(cmd_buffer[buf_tail]);

#ifdef LOW_SERVICE_RATE
        sleep(2); /* Simulate service time of 2 seconds */
#endif
     
        /* Move buf_tail forward, this is a circular queue */ 
        buf_tail++;
        if (buf_tail == CMD_BUF_SIZE)
            buf_tail = 0;

        pthread_cond_signal(&cmd_buf_not_full);
        /* Unlok the shared command queue */
        pthread_mutex_unlock(&cmd_queue_lock);
    } /* end for */
}
