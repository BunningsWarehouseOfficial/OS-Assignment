/* Filename:           lift_sim_B.c
   Author:             Kristian Rados (19764285)
   Date Created:       09/05/2020
   Date Last Modified: 17/05/2020
   Purpose: This is the primary process that controls the other processes. It houses the main() function where the other
            processes are created and managed. */

#define _GNU_SOURCE //Fixes VSCODE issue where 'MAP_ANONYMOUS' isn't recognised

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include "lift_sim_B.h"
#include "request_handler_B.h"
#include "lift_B.h"

/* PURPOSE: The main method for the simulation. It checks that the command parameters are valid before allocating
memory for all the necessary shared variables such as the buffer. It then creates the four processes and waits for them
to finish. */
int main(int argc, char* argv[]) {
    if (argc == 3) {
        //Creating the struct that will contain all the data that needs to be shared between the processes
        Shared* shared = (Shared*)sharedMMap(sizeof(Shared));

        if (loadSettings(shared, argv[1], argv[2]) == 1) {
            FILE* output;
            
            output = fopen("sim_out.txt", "w"); //Opening the output (logging) file
            if (output == NULL) {
                perror("Error opening sim_out.txt\n");
            }
            else {
                if (checkInput(&shared->remaining) == 1) {
                    int numLines;
                    sem_t mutex, full, empty;
                    sem_init(&mutex, 1, 1);
                    sem_init(&full, 1, 0);
                    sem_init(&empty, 1, shared->bufferSize);

                    //Creating the requests buffer
                    Request** buffer = (Request**)sharedMMap(shared->bufferSize * sizeof(Request*));
                    for (int ii = 0; ii < shared->bufferSize; ii++) {
                        buffer[ii] = (Request*)sharedMMap(sizeof(Request));
                    }
                    //Initialising the Shared struct
                    shared->output = output; 
                    shared->buffer = buffer;
                    shared->mutex = mutex;
                    shared->full = full;
                    shared->empty = empty;
                    numLines = shared->remaining; //The initial value of remaining represents the no. of lines in input

                    //Creating the Info structs that hold the data needed by the individual lift processes
                    Info** info = (Info**)sharedMMap(3 * sizeof(Info*));
                    for (int jj = 1; jj <= 3; jj++) {
                        info[jj - 1] = (Info*)sharedMMap(sizeof(Info));
                        info[jj - 1]->liftNo = jj;
                        info[jj - 1]->shared = shared;
                    }

                    if (fork() == 0) { //Creating the producer process
                        liftR((void*)shared);
                    }
                    for (int nn = 0; nn < 3; nn++) { //Creating the three consumer child processes
                        if (fork() == 0) {
                            lift((void*)info[nn]);
                        }
                    }

                    //Waiting for the processes to finish executing
                    for (int kk = 0; kk < 4; kk++) {
                        wait(NULL); //Join so that main waits until threads finish: start with id[0], ie. liftR
                    }
                    fprintf(output, "Total number of requests: %d\n", numLines); //Final two writes to output log
                    fprintf(output, "Total number of movements: %d", shared->combinedMovement);
                    printf("\nFinished simulation. \nOutput has been logged to 'sim_out.txt'.\n");

                    sem_destroy(&mutex);
                    sem_destroy(&full);
                    sem_destroy(&empty);
                    //Unmapping memory allocated to processes
                    for (int ll = 0; ll < shared->bufferSize; ll++) {
                        munmap(shared->buffer[ll], sizeof(Request));
                    }
                    munmap(buffer, sizeof(shared->bufferSize * sizeof(Request*)));
                    for (int mm = 0; mm < 3; mm++) {
                        munmap(info[mm], sizeof(Info));
                    }
                    munmap(info, 3 * sizeof(Info*));
                }
            }
            fclose(output);

        }
        munmap(shared, sizeof(Shared));
    }
    else { //Error for running executable on command line incorrectly
        printf("Error: Program must be run as 'lift_sim_A [m] [t]' where m is the buffer size and t is the time "
               "required per lift request\n");
    }
    return 0;
}

/* PURPOSE: It takes in the command line parameters and checks that they are valid. It returns a boolean value that
ends the program before the simulation starts if the parameters are not valid. If they are valid, then the Shared
struct values are initialised. */
int loadSettings(Shared* shared, char* a, char* b) {
    int scan1, scan2, bufferSize, requestTime, valid;
    valid = 0; //'boolean'
    scan1 = sscanf(a, "%d", &bufferSize);
    scan2 = sscanf(b, "%d", &requestTime);

    if (scan1 != 1 && scan2 != 1) {
        printf("Error: Command line parameters for buffer size [m] and the time required [t] must be two integers\n");
    }
    else if (bufferSize < 1) {
        printf("Error: Buffer size has been specified as %d, but it must be > 0\n", bufferSize);
    }
    else if (requestTime < 0) {
        printf("Error: The time taken per lift request has been specified as %d, but it must be >= 0\n", requestTime);
    }
    else { //Command line parameters have been validated, hence values are inserted into Shared struct
        shared->bufferSize = bufferSize;
        shared->requestTime = requestTime;
        shared->remaining = 0;
        shared->combinedMovement = 0;
        shared->index = 0;
        valid = 1;
    }
    return valid;
}

/* PURPOSE: Integrates the creation of shared memory into a function similar to malloc() so that the corresponding
mallocs in lift_sim_A can be substituted with this function. */
void* sharedMMap(size_t size) {
    int prot = PROT_READ | PROT_WRITE; //Processes can read and write to the shared memory
    int flags = MAP_SHARED | MAP_ANONYMOUS; //Is visible to other processes, but only those which are child processes
    return mmap(NULL, size, prot, flags, -1, 0);
}