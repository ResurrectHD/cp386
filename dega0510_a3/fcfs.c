/*  ************************************************************************************
    CP 386
    Assignment 3 - Q 2
    
    Authors: 
    Josh Degazio - 190560510
    Stany Chabikuli - 200137630

    Date: 2022-11-02

    FILE INFO
        Based on FCFS:
        Find the average waiting time and turn-around time for the
        pre-defined set of tasks; read from "sample_in_schedule.txt"
    ************************************************************************************
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>

//Create struct for threads
typedef struct thread{
    int p_id;
    int arr_time;
    int burst_time;
    int completion_time;
    int waiting_time;
    int turn_around_time;
} Thread;

//Read the file content and build array of threads
int readFile(char *fileName, Thread **threads); 
void fcfs(Thread **threads, int tCount);
void averages(Thread **threads, int tCount);
void createOutput(Thread **threads, int tCount);

//Initialize averages as globals
    float waitAvg;
    float turnaroundAvg;

int main(int argc, char *args[]) {

    //check if user inputted file name
    if (argc < 2)
	{
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}

    //Initialize threads
    Thread *threads = NULL;
    //Create threads from input file and get the count.
	int tCount = readFile(args[1], &threads);
    fcfs(&threads, tCount);
    averages(&threads, tCount);
    createOutput(&threads, tCount);

    //printf("%d, %d, %d, %d, %d\n", threads[2].p_id, threads[2].arr_time, threads[2].burst_time, threads[2].waiting_time, threads[2].turn_around_time);
    //printf("%d\n\n", tCount);
    return 0;
}

//Read input from file
int readFile(char *fileName, Thread **threads){
	FILE *inpt_file = fopen(fileName, "r");
	if (!inpt_file)
	{
		printf(
			"Error opening input file... Exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(inpt_file), &st);
	char *fileContent = (char *)malloc(((int)st.st_size + 1) * sizeof(char));
	fileContent[0] = '\0';
	while (!feof(inpt_file))
	{
		char line[32];
		if (fgets(line, 32, inpt_file) != NULL)
		{
			strncat(fileContent, line, strlen(line));
		}
	}
	fclose(inpt_file);

	int tCount = 0;
    char *command = NULL;
	char *fileCopy = (char *)malloc((strlen(fileContent) + 1) * sizeof(char));

	strcpy(fileCopy, fileContent);
	command = strtok(fileCopy, "\r\n");
	while (command != NULL)
	{
		tCount++;
		command = strtok(NULL, "\r\n");
	}
	*threads = (Thread *)malloc(sizeof(Thread) * tCount);

	char *lines[tCount];
    int i = 0;
	command = NULL;
	command = strtok(fileContent, "\r\n");

	while (command != NULL)
	{
		lines[i] = malloc(sizeof(command) * sizeof(char));
		strcpy(lines[i], command);
        command = strtok(NULL, "\r\n");
		i++;
	}

	for (int x = 0; x < tCount; x++)
	{
        int y = 0;
		char *token = NULL;
		token = strtok(lines[x], ", ");
		while (token != NULL)
		{
			if (y == 0)
				(*threads)[x].p_id = atoi(token);
			if (y == 1)
				(*threads)[x].arr_time = atoi(token);
            if (y == 2){
                (*threads)[x].burst_time = atoi(token);
            }
			y++;
			token = strtok(NULL, ", ");
		}
	}
	return tCount;
}

//calculate turn around and waiting times given input
void fcfs(Thread **threads, int tCount){
    int index = 0;
    int current_time = 0;

    while(index < tCount){
        if((*threads)[index].arr_time <= current_time){
            (*threads)[index].completion_time = current_time + (*threads)[index].burst_time;
        }
        else{
            (*threads)[index].completion_time = (*threads)[index].arr_time + (*threads)[index].burst_time;
        }

        (*threads)[index].turn_around_time = (*threads)[index].completion_time - (*threads)[index].arr_time;
        (*threads)[index].waiting_time = (*threads)[index].turn_around_time - (*threads)[index].burst_time;

        current_time = (*threads)[index].completion_time;
        index++;
    }
    return;
}

//Simply calculate averages, assigning them to the already initalized globals
void averages(Thread **threads, int tCount){
    int index = 0;
    float waitTime_Total = 0;
    float turnAround_Total = 0;

    while(index < tCount){
        waitTime_Total += (*threads)[index].waiting_time;
        turnAround_Total += (*threads)[index].turn_around_time;
        index++;
    }

    waitAvg = waitTime_Total/index;
    turnaroundAvg = turnAround_Total/index;

    return;
}

//Print all output to console
void createOutput(Thread **threads, int tCount){
    int index = 0;
    //line words up with given expected output
    printf("Thread ID         Burst Time      Arrival Time    Completion Time Turn-Around Time        Waiting Time\n");

    while(index < tCount){
        //line spacing up with the expected output given
        printf("%d %25d %15d %15d %15d %15d\n",
        (*threads)[index].p_id, (*threads)[index].burst_time, (*threads)[index].arr_time, (*threads)[index].completion_time, (*threads)[index].turn_around_time, (*threads)[index].waiting_time);
        index++;
    }

    //print average times
    printf("The average waiting time: %.2f\n", waitAvg);
    printf("The average turn-around time: %.2f\n", turnaroundAvg);

    return;
}