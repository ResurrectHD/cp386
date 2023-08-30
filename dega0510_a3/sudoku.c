/*  ************************************************************************************
    CP 386
    Assignment 3 - Q 1

    Authors:
    Josh Degazio - 190560510
    Stany Chabikuli - 200137630

    Date: 2022-11-02

    FILE INFO
		Read sudoku answer from "sample_in_sudoku.txt" and confirms if it is a valid
		solution. Uses multithreading to solve the columns, rows, and 3x3 grids
    ************************************************************************************
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>

/*OVERALL IDEA:
 * 1. 9 threads to check that each column contains the digits 1-9
 * 	     - If not then validity Array is not updated
 * 2. 9 threads to check that each row contains the digits 1-9
 * 	     - If not then validity Array is not updated
 * 3. 9 threads to check that each of the 3x3 grids contain digits 1-9
 * --------
 * 	 27 total threads
 *
 * 4. Have a validity array that will be updated with 1s after each check
 * 		- If the array is not filled with 1s then the solution is invalid
 */

/*structure for passing data to threads */
typedef struct{
	int row;
	int col;
}parameters;

/* structure for passing multiple arguments to threads */
typedef struct{
	parameters *data;
	int** array;
}arguments;

int validArr[27] = {0};	//number of threads long array filled with 0s

//Initialize methods
int **readFile(char *fileName);//reads file and builds 2d array of sudoku solution
void *colValid(void* val);
void *rowValid(void* val);
void *gridValid(void* val);

int main(int argc, char *args[]) {
	int i, j;
	if (argc < 2) //no filename was input by the user
	{
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}
	//intialize threads using readFile()
	char *fname = args[1];//args[1] filename passed by makefile
	int** sudoku = readFile(fname);

	printf("Sudoku Puzzle Solution is:\n");
	//print the puzzle
	for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			if(j == 8){
				printf("%d\n", sudoku[i][j]);
			}else{
				printf("%d ", sudoku[i][j]);
			}
		}
	}
	//now create 27 threads and call functions to update validity array
	pthread_t threadsArr[27];
	int indx = 0; //index for thread array

	for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			if(i == 0){ //row is 0 so we validate all columns
				parameters *data = (parameters *)malloc(sizeof(parameters));
				data-> row = i;
				data-> col = j;
				arguments *ar = (arguments *)malloc(sizeof(arguments));
				ar-> data = data;
				ar-> array = sudoku;
				/*Now create the thread passing its data as a parameter */
				pthread_create(&threadsArr[indx++], NULL, colValid, ar);
			}if(j == 0){//column is 0 so we validate all rows
				parameters *data = (parameters *)malloc(sizeof(parameters));
				data-> row = i;
				data-> col = j;
				arguments *ar = (arguments *)malloc(sizeof(arguments));
				ar-> data = data;
				ar-> array = sudoku;
				/*Now create the thread passing its data as a parameter */
				pthread_create(&threadsArr[indx++], NULL, rowValid, ar);
			}if(i % 3 == 0 && j % 3 == 0){//this block is only for every third row and column for 3x3 grids
				parameters *data = (parameters *)malloc(sizeof(parameters));
				data-> row = i;
				data-> col = j;

				arguments *ar = (arguments *)malloc(sizeof(arguments));
				ar-> data = data;
				ar-> array = sudoku;
				/*Now create the thread passing its data as a parameter */
				pthread_create(&threadsArr[indx++], NULL, gridValid, ar);
				//pthread_create(thread array, NULL, gridvalid subroutine(data, sudoku puzzle)
			}//INCREMENT each indx after each pthread creation to follow loop iteration
		}
	}
	//wait for all other threads to complete and terminate each thread with pthread_join loop
	for(i = 0; i < 27; i++){
		pthread_join(threadsArr[i], NULL);
	}
	//now check validity array
	for(i = 0; i < 27; i++){ //length of validarray
		if(validArr[i] == 0){//valid solution should have all values equal 1 in array
			printf("Sudoku puzzle is invalid\n");
			return EXIT_SUCCESS;
		}
	}
	printf("Sudoku puzzle is valid\n");


	return EXIT_SUCCESS;
}

int **readFile(char *fileName){
	FILE *inFile = fopen(fileName, "r");
	int **sudoku;
	int i, j;

	sudoku = malloc(sizeof(int*) *9);
	for(i = 0; i < 9; i++){
		sudoku[i] = malloc(sizeof(int*) *9);
	}

	for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			fscanf(inFile, "%d", &sudoku[i][j]);
		}
	}
	fclose(inFile);

	return sudoku;
}

//pass each method the sudoku array
void* rowValid(void* val){
	arguments *values = (arguments*)val;
	int col = values-> data-> col;
	int row = values->data-> row;
	int** sudoku = values ->array;

	//Row must be less than 9 and col must be 0
	if(row > 8 || col != 0){
		printf("Invalid row or column number for validity check\n");
		pthread_exit(NULL);
	}

	int checker[9] = {0}; //fill an array with 9 slots of 0s
	int i, x;

	for(i = 0; i < 9; i++){ //loop through sudoku row to ensure numbers 1-9 are present
		x = sudoku[i][row];
		if(x < 1 || x > 9|| checker[x-1] == 1){
			//if it already equals 1 then there is more than one instance of that number (DONT UPDATE JUST EXIT)
			pthread_exit(NULL);
		}else{
			checker[x - 1] = 1;
		}
	}
	validArr[9 + row] = 1;
	pthread_exit(NULL);
}
void* colValid(void* val){
	arguments *values = (arguments*)val;
	int col = values-> data-> col;
	int row = values->data-> row;
	int** sudoku = values ->array;

	//Column must be less than 9 and row must be 0
	if(col > 8 || row != 0){
		printf("Invalid row or column number for validity check\n");
		pthread_exit(NULL);
	}

	int checker[9] = {0}; //fill an array with 9 slots of 0s
	int i, x;

	for(i = 0; i < 9; i++){ //loop through sudoku column to ensure numbers 1-9 are present
		x = sudoku[i][col];
		if(x < 1 || x > 9|| checker[x-1] == 1){
			//if it already equals 1 then there is more than one instance of that number
			pthread_exit(NULL);
		}else{
			checker[x - 1] = 1;
		}
	}
	validArr[18 + col] = 1;
	pthread_exit(NULL);
}

void* gridValid(void* val){
	arguments *values = (arguments*)val;
	int col = values-> data-> col;
	int row = values->data-> row;
	int** sudoku = values ->array;

	if(row > 6 || row % 3 != 0 || col > 6 || col % 3!= 0){
		printf("Invalid row or column number for validity check\n");
		pthread_exit(NULL);
	}

	int checker[9] = {0}; //fill an array with 9 slots of 0s
	int i, j, x;

	for(i = row; i < row + 3; i++){ //loop through 3 rows
		for(j = col; j < col + 3; j++){ //loop through 3 columns
			x = sudoku[i][j];
			if(x < 1 || x > 9 || checker[x - 1] == 1){
				//if it already equals 1 then there is more than one instance of that number so exit!
				pthread_exit(NULL);
			}else{
				checker[x - 1] = 1;
			}
		}
	}

	validArr[col + row/3] = 1; //first 9 slots of valid array = 1 if valid array
	pthread_exit(NULL);

}




