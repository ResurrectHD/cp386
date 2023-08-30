/*  ************************************************************************************
    CP 386
    Assignment 2 - Question 2

    Authors:
    Josh Degazio - 190560510
    Stany Chabikuli - 200137630

    Date: 2022-10-19

    FILE INFO
        Parent process using fork systems calls for creating children processes, whenever required, and collecting the output of these.
        Furthermore, the children processes that are created will end up outputting contents of the file "sample_in_process.txt".
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

//Initialize methods
int main(int argc, char *argv[]);
void readFile(char *shm_ptr, char *filename, int length);
void exec_cmds_from_shm(char *memory_pointer);
void pipe_to_file(int pipe_ID);
void write_cmds_to_pipe(char *ptr, int pipe_ID);

//Initialize constants
const char *OUTPUT_FNAME = "output.txt";
const int SIZE = 4096;
const char *SHM_FNAME = "SHM_FILE";

/*
Used alternative method to using dup2(). Discussed with professor and it's ok
*/

//My main B
int main(int argc, char *args[])
{
    char *shm_ptr;
    int shm_fd;

    //Openn the doorrrrr
    shm_fd = shm_open(SHM_FNAME, O_CREAT | O_RDWR, 0666);

    //Nononononn
    if (shm_fd < 0){
        //Another error :/
        printf("**ERROR** Shared Memory failed to open\n");
        exit(-1);
    }

    //Configuring size
    ftruncate(shm_fd, SIZE);

    //Memory mapping shared memory object
    shm_ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    //Something wrongggg
    if (shm_ptr == MAP_FAILED){
        //Error :(
        printf("**ERROR** Map failed\n");
        exit(-1);
    }

    //Command line args
    char *fname = args[1];

    //Child process reading from da file
    int child_process_id = fork();

    if (child_process_id == 0){
        //printf("this is a child process");
        readFile(shm_ptr, fname, strlen(fname));
        exec_cmds_from_shm(shm_ptr);

        //Unmap and unlink da shared memory
        munmap(shm_ptr, SIZE);
        shm_unlink(SHM_FNAME);

        exit(0);
    }
    //Something wrong
    else if (child_process_id == -1){
        //ERRRORRRR
        printf("ERROR: Creation of the child process has failed");
        exit(-1);
    }
    return(0);
}

void exec_cmds_from_shm(char *memory_pointer) {
    char *ptr = memory_pointer;

    //Create da pipe, name it, and create da child process.
    char *pipe_name = "/tmp/mypipe";
    mkfifo(pipe_name, 0666);
    pid_t process_ID = fork();

    //Child Process
    if (process_ID == 0) {
        char result[SIZE];
        //Open the pipe in da RIGHT mode ;)
        int pipe_ID = open(pipe_name, O_WRONLY);
        for (int i = 0; i < 96; i += 1) {
            char data = (char)ptr[i];
            strncat(result, &data, 1);
        }

        char *result_ptr = strtok(result, "\r\n");

        //dup2(result_ptr,0)
        write_cmds_to_pipe(result_ptr, pipe_ID);
        close(pipe_ID);
    }

    //Parent Process
    else if (process_ID > 0) {
        int pipe_ID = open(pipe_name, O_RDONLY);
        int status;
        wait(&status);
        if (WEXITSTATUS(status) == -1) {
            //ERRRORRRR!!!
            perror("\n**ERROR** waiting\n");
            exit(-1);
        }
        // Creates connection from end of da pipe to da file, then closes
        pipe_to_file(pipe_ID);
        close(pipe_ID);
    }

    //Something went wrong :/
    else {
        //ERROR!!!
        printf("\n**ERROR** While forking\n");
        exit(-1);
    }
}

void readFile(char *shm_ptr, char* fName, int file_length)
{
    //Create da file variable and open it in da reading mode
    FILE *fp;
    fp = fopen(fName, "r");

    size_t len_of_line = 0;
    ssize_t read;
    char *line = NULL;

    //While a new line exists, read it to da shared memory.
    while ((read = getline(&line, &len_of_line, fp)) != -1)
    {
        shm_ptr += sprintf(shm_ptr, "%s", line);
    }
}

void pipe_to_file(int pipe_ID) {
    //Get da pipe ready to read
    char result[SIZE];
    read(pipe_ID, result, SIZE);

    FILE *output_file = fopen(OUTPUT_FNAME, "w");
    char *ptr = strtok(result, "\r\n");

    //Sends text to file
    while (ptr) {
        fprintf(output_file, "%s\n", ptr);
        ptr = strtok(NULL, "\r\n");
    }
    //Close
    fclose(output_file);
}

void write_cmds_to_pipe(char *ptr, int pipe_ID) {
    char full_output[SIZE];
    int temp = 0;

    //while ptr isnt empty
    while (ptr) {
        temp = temp+1;
        //open da virtual file in da reading mode
        FILE *virtual_file = popen(ptr, "r");


        char line[1035];

        //if virtual file basically isnt null
        if (virtual_file) {
            char first_line[50];

            if(strchr(ptr, '>') == NULL){
                //here da output
                sprintf(first_line, "\nThe output of: %s : is\n", ptr);
                //add the arrows
                //printf("\nThis is while loop: %d\n This is ptr: %s\n", temp, ptr);
                strcat(first_line, ">>>>>>>>>>>>>>>\n");
                strcat(full_output, first_line);

                while (fgets(line, sizeof(line), virtual_file) != NULL) {
                    strcat(line, "\n");
                    strcat(full_output, line);
                }

                //add the other arrows
                strcat(full_output, "<<<<<<<<<<<<<<<");
            }
        //or elseeeeee
        }

        else {
            printf("\nERROR: Issue executing '%s'!\n", ptr);
            exit(-1);
        }
        //close da virtual file
        fclose(virtual_file);
        ptr = strtok(NULL, "\r\n");
    }

    //write that down!!!
    write(pipe_ID, full_output, SIZE + 1);
}
