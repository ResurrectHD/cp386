/*  ************************************************************************************
    CP 386
    Assignment 2 - Question 1

    Authors:
    Josh Degazio - 190560510
    Stany Chabikuli - 200137630

    Date: 2022-10-19

    FILE INFO
    	Teacher Process is the parent process of this file. It creates 3 GTA processes which
    	each create 2 TA processes that are responsible for getting the averages of one assignment each
    	(6 total assignments) Grade information is found in "sample_in_grades.txt"
    ************************************************************************************
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main(int argc, char *args[]){
	FILE* samples;

	//open sample grades file for reading
	samples = fopen("sample_in_grades.txt", "r");

	if(samples == NULL)//file is empty
		printf("File could not be opened");

	//create  2d matrix for grades 6 columns by 10 rows
	int gradesArr[10][6];
	int i, j;
	// fill grades array with the file info
	for(i = 0; i < 10; i++){
		for(j = 0; j < 6; j++){
			fscanf(samples, "%d", &gradesArr[i][j]);
		}
	}
	//create the teacher gradTa and ta child processes
	int pid, pid2;
	int row, col;
	float avg = 0;
	int assignment = 1; // starting assignment

	//This for loop creates the 3 GradTa processes
	for(i = 0; i < 3; i++){
		pid = fork();
		if(pid == 0){//GTA process was made
			break;
		}
		else if(pid > 0){
			wait(NULL);
		}
	}

	//this if checks to make sure we are currently in the GradTA Processes
	if(pid == 0){// if I am a gradTA process
		for(j = 0; j < 6; j++){
			pid2 = fork(); //create 6 TA processes under gradTa
			if(pid2 == 0){
				break;
			}else if(pid > 0){
				wait(NULL);
			}
		}
	}
	// this if block is to check if each TA process is the current child
	// so it only runs if the gradTa is the parent and not the current child
	if(pid2 == 0 && pid > 0){
		for(col = 0; col < 6; col++){ //iterate through each assignment
			for(row = 0; row < 10; row++){ //iterate through each grade
				avg += gradesArr[row][col];
			}
			avg = avg / 10;
			printf("Assignment %d - Average = %0.5f\n", assignment, avg);
			assignment += 1;
			avg = 0;
		}
	}

	return 0;
}

