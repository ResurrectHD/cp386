/*  ************************************************************************************
    CP 386
    Assignment 1 - Q 2
    
    Author: 
    Josh Degazio - 190560510

    Date: 2022-09-21

    FILE INFO
        Copies the contents of one file to a destination file
    ************************************************************************************
*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

//declare for later use
void copydafile();
char temp_c;
FILE * f_inpt;
FILE * f_otpt;

int main(int argc, char *argv[]){
    if( argc == 3 ) {
        //open file streams
        remove(argv[2]);
        f_inpt  = fopen(argv[1], "r");
        f_otpt  = fopen(argv[2],   "w");
        //process information
        copydafile();
        //close file streams
        fclose(f_inpt);
        fclose(f_otpt);
        //success
        printf("The contents of file %s have been successfully copied into the %s file.", argv[1], argv[2]);
    }

    //how do you do too much
    else if( argc > 3 ) {
        printf("Too many arguments supplied.\n");
    }
    //not doing enough is more likely.
    //let them know they messed up
    else {
        printf("Insufficient parameters passed.\n");
    }

    return 0;
}

void copydafile(){
    //Input doesn't exist or privileges are insufficient
    if (f_inpt == NULL || f_otpt == NULL)
    {
        //tell the user
        printf("Input either doesn't exist or privileges are insufficient. \n");

        //wheezy outta here
        exit(EXIT_FAILURE);
    }

    //start reading file
    temp_c = fgetc(f_inpt);

    //read and write contents
    while (temp_c != EOF)
    {
        //write
        fputc(temp_c, f_otpt);

        //read
        temp_c = fgetc(f_inpt);
    }
}
