#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct CPUData {
	
	int p_id;
	int burstTime;
	int arrivalTime;
	int priority;
	
	struct CPUData *next;
};

struct List {
	
    struct CPUData *head_ref;
	
};

struct Queue {
	
	int p_id;
	int burstTime;
	int arrivalTime;
	int priority;
	
	int size;
};

void sorter(const void *arrivaltimeA, const void *arrivalTimeB)
{
	return strcmp (((struct CPUData*)arrivaltimeA)-> arrivalTime,
	((struct CPUData*)arrivalTimeB)-> arrivalTime);
}

void readFile (struct queue *queue)
{
	FILE *readfile;
	char filename[2] = "1M";
	
	size_t len = 0;
    ssize_t read;
	 char *line = NULL;
	 struct List *list = (struct List *) malloc(sizeof(struct List));
	
	readfile = fopen(filename, "r");
	if(readfile==NULL)
	{
		printf("\n Error, file could not be opened \n");
		exit(EXIT_FAILURE);
	}
	
		while ((read = fgets(&line, &len, readfile)) !=1)
		{
		char *token = strtok (line, "\t"); //tokenizes the input from the file and stores it accordingly
		int pid = atoi(token); //puts the pid into the token
        int burst = 0;
        int arrival = 0;
		int prio = 0;
		
		while(token != NULL)
		{	
		token = strtok (NULL, "\t"); 
		if(NULL !=token)
		{
			burst = atoi(token); //inserts the value into burst
		}
		token = strtoken(NULL, "\n");
		if(NULL != token)
		{
			arrival = atoi(token); //inserts the value into arrival
		}
		token = strtoken(NULL, "\n"); //inserts the value into priority
		if(NULL != token)
		{
			prio = atoi(token);
		}
		} //end of inner while loop
		
		printf("%d, %d, %d, %d, pid" , burst, arrival, prio);
		
		} //end of read line while loop
	
	
}
