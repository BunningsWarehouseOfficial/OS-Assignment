/* Filename:           lift_sim_A.c
   Author:             Kristian Rados (19764285)
   Date Created:       19/04/2020
   Date Last Modified: 17/05/2020
   Purpose: This is the primary thread that controls the other threads. It houses the main() function where the other
            threads are created and managed. */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lift_sim_A.h"
#include "request_handler_A.h"
#include "lift_A.h"

/* PURPOSE: The main method for the simulation. It checks that the command parameters are valid before allocating
memory for all the necessary shared variables such as the buffer. It then creates the four threads and waits for them 
to finish. */
int main(int argc, char* argv[]) {
    if (argc == 3) {
        //Creating the struct that will contain all the data that needs to be shared between the threads
        Shared* shared = (Shared*)malloc(sizeof(Shared));

        if (loadSettings(shared, argv[1], argv[2]) == 1) {
            FILE* output;
            
            output = fopen("sim_out.txt", "w"); //Opening the output (logging) file
            if (output == NULL) {
                perror("Error opening sim_out.txt\n");
            }
            else {
                if (checkInput(&shared->remaining) == 1) {
                    int numLines;
                    pthread_t id[4];
                    pthread_mutex_t bufferLock = PTHREAD_MUTEX_INITIALIZER;
                    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
                    Info** info = (Info**)malloc(3 * sizeof(Info*)); //Array of info structs to distribute to threads

                    //Creating the requests buffer
                    Request** buffer = (Request**)malloc(shared->bufferSize * sizeof(Request*));
                    for (int ii = 0; ii < shared->bufferSize; ii++) {
                        buffer[ii] = (Request*)malloc(sizeof(Request));
                    }
                    //Initialising the Shared struct
                    shared->output = output; 
                    shared->buffer = buffer;
                    shared->bufferLock = bufferLock;
                    shared->cond = cond;
                    numLines = shared->remaining; //The initial value of remaining represents the no. of lines in input

                    //Creating the threads
                    pthread_create(&id[0], NULL, liftR, (void*)shared);
                    for (int jj = 1; jj <= 3; jj++) {
                        //Initialising the Info structs that hold the data needed by the individual lift threads
                        info[jj - 1] = (Info*)malloc(sizeof(Info));
                        info[jj - 1]->liftNo = jj;
                        info[jj - 1]->shared = shared;
                        pthread_create(&id[jj], NULL, lift, (void*)info[jj - 1]);
                    }

                    //Waiting for the threads to finish executing
                    for (int kk = 0; kk < 4; kk++) {
                        pthread_join(id[kk], NULL); //Join so main waits until threads finish, start with id[0], liftR
                    }
                    fprintf(output, "Total number of requests: %d\n", numLines); //Final two writes to output log
                    fprintf(output, "Total number of movements: %d", shared->combinedMovement);
                    printf("\nFinished simulation. \nOutput has been logged to 'sim_out.txt'.\n");

                    pthread_mutex_destroy(&bufferLock);
                    pthread_cond_destroy(&cond);
                    //Freeing memory allocated to threads
                    for (int ll = 0; ll < shared->bufferSize; ll++) {
                        free(shared->buffer[ll]);
                    }
                    free(buffer);
                    for (int mm = 0; mm < 3; mm++) {
                        free(info[mm]);
                    }
                    free(info);
                }
                fclose(output);
            }
        }
        free(shared);
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
        shared->empty = shared->bufferSize; //Initialising empty as size of buffer, indicating that the buffer is empty
        shared->remaining = 0;
        shared->combinedMovement = 0;
        valid = 1;
    }
    return valid;
}