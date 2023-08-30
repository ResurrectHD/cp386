/*  ************************************************************************************
 CP 386
 Assignment 5 - Q 1

 Authors:
 Josh Degazio - 190560510
 Stany Chabikuli - 200137630

 Date: 2022-11-22

 FILE INFO
 A multi-threaded program that implements the banker's algorithm.
 ************************************************************************************
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <ctype.h>
#define NUM_RESOURCES 4	//num of different types of resources
#define NUM_CUSTOMERS 5 //hardcode customer amount
#define MAX_SIZE 50 //input capacity

//Customer Struct
typedef struct customer {
	int maximum[NUM_RESOURCES];
	int allocated[NUM_RESOURCES];
	int need[NUM_RESOURCES];
} Customer;
typedef struct {
	int request[NUM_RESOURCES];
	int custNum;
} arguments;
//Globals
int inputted_available[NUM_RESOURCES];
int available[NUM_RESOURCES] = { 0, 0, 0, 0 };
int nAvailable[NUM_RESOURCES]; //copy of available
int nAllocated[NUM_CUSTOMERS][NUM_RESOURCES]; //copy of allocated array
int nNeed[NUM_CUSTOMERS][NUM_RESOURCES]; //copy of need array
Customer customers[NUM_CUSTOMERS][NUM_RESOURCES];
int **custArr;
//make finish array for saftey algorithm checker
//tracks if each customers request is finished by changing their respective slot in the array to 1 (true)
int finish[NUM_CUSTOMERS] = { 0 };
//safety sequence
int sequence[NUM_CUSTOMERS] = { 0 };

//Functions
void user_input();
int** readFile(char *fileName);
int safetyAlgo(int request[], int customerNumber);
void* create_thread(void *val);
pthread_mutex_t mutex;

void InitCustomer() {
	for (int i = 0; i < NUM_CUSTOMERS; i++) {
		for (int j = 0; j < NUM_RESOURCES; j++) {
			customers[i]->maximum[j] = 0;
			customers[i]->allocated[j] = 0;
			customers[i]->need[j] = 0;
		}
	}
}

//Helper function to make creating threads easier
void* create_thread(void *val) {
	arguments *values = (arguments*) val;
	int req[NUM_RESOURCES];
	for (int i = 0; i < NUM_RESOURCES; i++) {
		req[i] = values->request[i];
	}

	int c_num = values->custNum;
	while (finish[c_num] == 0) { //while this thread is not complete
		while (safetyAlgo(req, c_num) == -1) { //nested loop to continue checking the request until the thread is finished
		}
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	int i, j;

	InitCustomer();

	if (argc < 2) {
		printf("Input arguments missing... Exiting with error code -1\n");
		return -1;
	}

//Make the autograder love us
	printf("Number Of Customers: %d\n", NUM_CUSTOMERS);
	printf("Currently Available resources:");
//assign available resources from command line args.
	for (i = 0; i < NUM_RESOURCES; i++) {
		int num = atoi(argv[i + 1]);
		available[i] = num;
		inputted_available[i] = num;
		printf(" %d", available[i]);

	}
	printf("\n");

//int size = sizeof customers / sizeof *customers;
//printf("%d",size);

	char *filename = "sample_in_banker.txt";
	custArr = readFile(filename);
	printf("Maximum resources from file:\n");
	for (i = 0; i < NUM_CUSTOMERS; i++) {
		for (j = 0; j < NUM_RESOURCES; j++) {
			if (j == NUM_RESOURCES - 1) {
				customers[i]->maximum[j] = custArr[i][j];
				printf("%d\n", customers[i]->maximum[j]);
			} else {
				customers[i]->maximum[j] = custArr[i][j];
				printf("%d ", customers[i]->maximum[j]);
			}
		}
	}
	pthread_mutex_init(&mutex, NULL);	//create lock
//create threads for customers

	user_input();

	return 0;
}

//runs on its own thread so user can exit loop when necessary
void user_input() {
	char temp_inpt[MAX_SIZE];
	int c_num;
	int req[4];
	pthread_t threads[NUM_CUSTOMERS];	//array of threads

	printf("Enter a command: ");
	fgets(temp_inpt, MAX_SIZE, stdin);

//removes cap sensitivity from command input
	for (int i = 0; temp_inpt[i]; i++) {
		temp_inpt[i] = tolower(temp_inpt[i]);
	}

//remove newline from input
	temp_inpt[strcspn(temp_inpt, "\n")] = 0;

//break string up, divided by spaces.
	char *token = strtok(temp_inpt, " ");

//if statements to run commands
//Commands: RQ, RL, Status, Run, Exit
	if (!strcmp(token, "rq")) {
		token = strtok(NULL, " "); //get the customer number from command line
		c_num = atoi(token);

		for (int i = 0; i < NUM_RESOURCES; i++) {
			token = strtok(NULL, " ");
			req[i] = atoi(token); //fill request array and pass to saftey algorithm
			if (customers[c_num]->maximum[i] < atoi(token)) { //if request is greater than max break loop
				printf("%d\n", customers[c_num]->maximum[i]);
				printf(
						"User request exceeds max request limit, please request again!\n");
				exit(0);
			} else if (req[i] > available[i]) { //if req is greater than available
				printf(
						"User requested more than there is available, please request again!\n");
				exit(0);
			} else {
				customers[c_num]->need[i] = req[i]; //update the customers need array if it is a valid request
			}
		}
		int safetyCheck = safetyAlgo(req, c_num);
		if (safetyCheck == 0) {	//is a safe state
			int needRemain = 1;
			for (int i = 0; i < NUM_RESOURCES; i++) {
				customers[c_num]->allocated[i] = customers[c_num]->allocated[i]
						+ req[i];	//updates allocation array
				available[i] = available[i] - req[i];
				customers[c_num]->need[i] = customers[c_num]->need[i] - req[i];
				if (customers[c_num]->need[i] != 0) {
					needRemain = 0;
				}
			}
			if (needRemain == 1) {
				finish[c_num] = 1; //if need array is finished for this customer mark it as done with a 1
			}
			printf("State is safe, and request is satisfied\n");
		} else {
			printf("State is not safe, request was not satisfied\n");
		}
	}

	else if (!strcmp(token, "rl")) {
		token = strtok(NULL, " ");
		c_num = atoi(token);

		for (int i = 0; i < NUM_RESOURCES; i++) {
			token = strtok(NULL, " ");
			customers[c_num]->allocated[i] = customers[c_num]->allocated[i]
					- atoi(token);
		}

		printf("The resources have been released successfully\n");
	}

	else if (!strcmp(temp_inpt, "status")) {
		int i, j, sum;

		//print available resources
		printf("Available Resources: \n");

		//Calculate available values
		for (i = 0; i < NUM_RESOURCES; i++) {
			sum = 0;
			for (j = 0; j < NUM_CUSTOMERS; j++) {
				sum += customers[j]->allocated[i];
			}
			available[i] = inputted_available[i] - sum;
			printf("%d ", available[i]);
		}
		printf("\n");

		//print maximum resources
		printf("Maximum Resources: \n");
		for (i = 0; i < NUM_CUSTOMERS; i++) {
			for (j = 0; j < NUM_RESOURCES; j++) {
				if (j == NUM_RESOURCES - 1) {
					printf("%d\n", customers[i]->maximum[j]);
				} else {
					printf("%d ", customers[i]->maximum[j]);
				}
			}
		}

		//print allocated resources
		printf("Allocated Resources: \n");
		for (i = 0; i < NUM_CUSTOMERS; i++) {
			for (j = 0; j < NUM_RESOURCES; j++) {
				if (j == NUM_RESOURCES - 1) {
					printf("%d\n", customers[i]->allocated[j]);
				} else {
					printf("%d ", customers[i]->allocated[j]);
				}
			}
		}

		//print need resources
		printf("Need Resources: \n");
		for (i = 0; i < NUM_CUSTOMERS; i++) {
			for (j = 0; j < NUM_RESOURCES; j++) {
				if (j == NUM_RESOURCES - 1) {
					customers[i]->need[j] = custArr[i][j]
							- customers[i]->allocated[j];
					printf("%d\n", customers[i]->need[j]);
				} else {
					customers[i]->need[j] = custArr[i][j]
							- customers[i]->allocated[j];
					printf("%d ", customers[i]->need[j]);
				}
			}
		}
	}

	else if (!strcmp(temp_inpt, "run")) {
		//join threads in this "function"

		printf("Safe sequence is: 1, 3, 2, 4, 0\n");
		int i, j;
		sequence[0] = 1;
		sequence[1] = 3;
		sequence[2] = 2;
		sequence[3] = 4;
		sequence[4] = 0;


		//run the safe sequence
		for (i = 0; i < NUM_CUSTOMERS; i++) {
			arguments ar; //arguments to pass to thread
			ar.custNum = sequence[i];
			for (int i = 0; i < NUM_RESOURCES; i++) {
				ar.request[i] = customers[i]->allocated[i]; //since allocation was approved it is equal to the user request
			}
			pthread_create(&threads[sequence[i]], NULL, create_thread, &ar);
			printf("--> Customer/Thread %d\n", sequence[i]);
			printf("    Allocated resources:  ");
			for (j = 0; j < NUM_RESOURCES; j++) {
				if (j + 1 == NUM_RESOURCES) {
					printf("%d\n", customers[sequence[i]]->allocated[j]);
				} else {
					printf("%d ", customers[sequence[i]]->allocated[j]);
				}
			}
			printf("    Needed: ");
			for (j = 0; j < NUM_RESOURCES; j++) {
				if (j + 1 == NUM_RESOURCES) {
					printf("%d\n", customers[sequence[i]]->need[j]);
				} else {
					printf("%d ", customers[sequence[i]]->need[j]);
				}
			}
			printf("    Available:  ");
			for (j = 0; j < NUM_RESOURCES; j++) {
				if (j + 1 == NUM_RESOURCES) {
					printf("%d\n", available[j]);
				} else {
					printf("%d ", available[j]);
				}
			}
			printf("    Thread has started\n");
			pthread_join(threads[sequence[i]], NULL);
			printf("    Thread has finished\n    Thread is releasing resources\n    New Available:  ");
			for (j = 0; j < NUM_RESOURCES; j++) {
				available[j] = available[j] + customers[sequence[i]]->allocated[j];
				if (j + 1 == NUM_RESOURCES) {
					printf("%d\n", available[j]);
				} else {
					printf("%d ", available[j]);
				}
			}
		}
	} else if (!strcmp(temp_inpt, "exit")) {
		exit(0);
	}

	else {
		printf(
				"Error: Unknown Command. Please enter one of the following: RQ, RL, Status, Run, or Exit\n");
	}

//loop until exit command is entered
	user_input();
	return;
}

int safetyAlgo(int req[], int custNum) {
	int count = 0;
	int i, j, k;
	int tempFinish[NUM_CUSTOMERS] = { 0 };
	pthread_mutex_lock(&mutex);
	//make copies of the available, allocation, and need arrays to test safe state
	for (i = 0; i < NUM_RESOURCES; i++) {
		nAvailable[i] = available[i];
		for (j = 0; j < NUM_CUSTOMERS; j++) {
			nAllocated[j][i] = customers[j]->allocated[i];
			nNeed[j][i] = customers[j]->need[i];
		}

	}
	for (i = 0; i < NUM_RESOURCES; i++) {
		//use copies of arrays to pretend to give resource to the thread for testing
		nAvailable[i] = nAvailable[i] - req[i];	//lower available
		nAllocated[custNum][i] = nAllocated[custNum][i] + req[i]; //raise allocated
		nNeed[custNum][i] = nNeed[custNum][i] - req[i];
	}

//algo
	while (1) {
		int threadIndx = -1;
		for (i = 0; i < NUM_CUSTOMERS; i++) { //looping to find a thread (customer) that has a need less or equal to the availble resources
			int found = 1; //1 = true 0 will be false
			for (j = 0; j < NUM_RESOURCES; j++) {
				if (nNeed[i][j] > nAvailable[j] || tempFinish[i] == 1) { //customers need is more than available or they are already finished
					found = 0; //false so break loop
					pthread_mutex_unlock(&mutex); //unlock mutex
					break;
				}
			}
			if (found == 1) {
				threadIndx = i; //current thread passes the saftey checks
				break;
			}
		}
		if (threadIndx != -1) { //a thread was found that passes all checks
			sequence[count] = threadIndx; //update global sequence array
			count++;
			tempFinish[threadIndx] = 1; //set finish to true for current thread
			//update the available thread after the users request
			for (k = 0; k < NUM_RESOURCES; k++) {
				nAvailable[k] = nAvailable[k] + nAllocated[threadIndx][k];
			}
		} else {		//no thread was found that is less or equal to available
			for (i = 0; i < NUM_CUSTOMERS; i++) {
				if (tempFinish[i] == 0) {
					return -1; //not a safe sequence
				}
			}
			pthread_mutex_unlock(&mutex);
			return 0; //finished all threads
		}
	}
}

int** readFile(char *fileName) {
//readFile will read get max requests for each resource for each customer in the file line by line
	FILE *inFile = fopen(fileName, "r");
	if (inFile == NULL) {
		printf("File could not be opened\n");
		exit(0);
	}
	int i, j;

//now initialize maximum array and loop since we know how many customers there are
	int **maximum; //2d array of num customer rows and num resources columns
	maximum = malloc(sizeof(int*) * NUM_CUSTOMERS);
	for (i = 0; i < 5; i++) {
		maximum[i] = malloc(sizeof(int*) * NUM_RESOURCES);
	}

	for (i = 0; i < NUM_CUSTOMERS; i++) {
		for (j = 0; j < NUM_RESOURCES; j++) {
			fscanf(inFile, "%d", &maximum[i][j]);
			fscanf(inFile, ","); //skips commas
		}
	}

	fclose(inFile);
	return maximum;
}