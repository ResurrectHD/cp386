/*  ************************************************************************************
    CP 386
    Assignment 1 - Q 1
    
    Author: 
    Josh Degazio - 190560510

    Date: 2022-09-21

    FILE INFO
        Through users instructions; creates and removes directories,
        gets current directory, as well as changes, reads or closes directories.
        All of which, is based on user input.
    ************************************************************************************
*/
//includes
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <dirent.h>

//declare these boys
void check_input(char);
char u_inpt();
DIR *Opendrc;

//main
int main() {
    //check what da user said and then check what they said
    check_input(u_inpt());

    return 0;
}

//basically the user interface.
char u_inpt(){
        char temp_inpt;

        //give the user alllll the options
        printf("Select the option(s) appropriately be entering the number:\n");
        printf("Enter 1 for creating a directory\n");
        printf("Enter 2 for removing directory\n");
        printf("Enter 3 for printing working directory\n");
        printf("Enter 4 for changing directory one level up\n");
        printf("Enter 5 for reading the contents of directory\n");
        printf("Enter 6 for closing the directory\n");
        printf("Enter q to exit the program\n");
        //Ayo, what da user sayin??
        scanf("%s", &temp_inpt);
        printf("\n");

        //tell the program what da user said
        return temp_inpt;
    }

//create
void create_drc(){
    //need these
    char tempDrc[20];
    struct stat st = {0};

    //ask da user
    printf("Enter the Directory name you want to create:\n");
    scanf("%s", tempDrc);
    
    //if the directory doesnt exist
    if(stat(tempDrc, &st) == -1){
        //create it
        mkdir(tempDrc, 0700); //mkdir needs the second argument for Linux
        printf("Directory is Created Successfully.\n\n");
    }
    else{
        //uhoh
        printf("Something went wrong. Please try again.\n\n");
    }

    //looooop back
    main();
}

//remove
void remove_drc(){
    //gotta set these up again
    char tempDrc[20];
    struct stat st = {0};

    //whats up user??
    printf("Enter the Directory name you want to remove:\n");
    scanf("%s", tempDrc);

    //check that the directory exists
    if(stat(tempDrc, &st) == 0){
        //remove dat bruh
        rmdir(tempDrc);
        printf("Directory is removed Successfully.\n\n");
    }
    else{
        //not again
        printf("Something went wrong. Please try again.\n\n");
    }

    //loop it back
    main();

}

//print
void print_drc(){
    //need this for directory
    char cwd[400];
    // get the directory, as long as it isnt null
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        //this da directory
        printf("Current Working Directory is: ");
        puts(cwd);
    } 
    else {
        //Whole Lotta Red, I mean errors
        printf("Something went wrong. Please try again.");
    }

    //look pretty
    printf("\n\n");
    main();
}

//change
void change_drc(){
        char cwd[400];
        //check it again
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            //this da directory
            printf("Working Directory Before Operation: ");
            puts(cwd);

            //change da directory
            printf("Directory Changed Successfully.\n");
            chdir("..");

            //now dis da directory
            printf("Working Directory After Operation: ");
            puts(getcwd(cwd, sizeof(cwd)));

        } 
        else {
            //Error 404: Error Not Found
            printf("Something went wrong. Please try again.");
        }
        
        //Look good for the UI and loop
        printf("\n\n");
        main();
    }

//read
void read_drc(){
    //directory struct
    struct dirent *dir;
    //open the dooooor
    Opendrc = opendir(".");
    //if not null
    if (Opendrc) {
        //read the directory and print file names
        while ((dir = readdir(Opendrc)) != NULL) {
        printf("%s\n", dir->d_name);
        }
    }

    //looooooooooopdelooop
    main();
}

//close
void close_drc(){
    //close
    closedir(Opendrc);
    //close
    printf("Directory Closed Successfully.\n\n");

    //loop
    main();
}

//check what da user sayin
void check_input(char temp_inpt){
    //he want to create!!
        if(temp_inpt == '1'){
            create_drc();
        }
    //yall not rockin wit da user?!?
        else if(temp_inpt == '2'){
            remove_drc();
        }
    //print these bandz
        else if(temp_inpt == '3'){
            print_drc();
        }
    //da user tryna change up on us fr
        else if(temp_inpt == '4'){
            change_drc();
        }
    //I never learned how to:
        else if(temp_inpt == '5'){
            read_drc();
        }
    //To be a good business man you have to be able to:
        else if(temp_inpt == '6'){
            close_drc();
        }
    //after all these bad jokes I'm gonna have to:
        else if(temp_inpt == 'q'){
            exit(0);
        }
        //sike, another error
        else{
            printf("Something went wrong. Please try again.\n");

            //gotchu bro
            scanf("%s", &temp_inpt);
            //now whatchu sayin??
            check_input(temp_inpt);
        }
    }