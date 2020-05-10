/* Filename:           lift_sim_A.c
   Author:             Kristian Rados (19764285)
   Date Created:       19/04/2020
   Date Last Modified: 19/04/2020
   Purpose:                                                                                                           */

//TODO change back executable names to full names

//TODO give each thread an 'info' struct, kind of like the settings struct in UCP assignment

//TODO buffer as an array of pointers to request structs

//TODO merge this file for sim_A and sim_B if possible

//TODO Intro comment text for each .c file

//TODO 'PURPOSE:' before each function... just follow same commenting format as UCP assigment

//TODO Comment explaining any error message prints

//TODO Make sure I don't have any redundant #include statements

//TODO test checkInput

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "lift_sim_A.h"
#include "request_handler.h"
#include "lift.h"

int main(int argc, char* argv[]) {
    if (argc == 3) {
        Shared* shared = (Shared*)malloc(sizeof(Shared));

        if (loadSettings(shared, argv[1], argv[2]) == 1) {
            FILE* output;
            
            output = fopen("sim_out.txt", "w");
            if (output == NULL) {
                perror("Error opening sim_out.txt\n");
            }
            else {
                if (checkInput(&shared->remaining) == 1) {
                    int numLines;
                    pthread_t id[4];
                    pthread_mutex_t bufferLock = PTHREAD_MUTEX_INITIALIZER;
                    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
                    Info** info = (Info**)malloc(3 * sizeof(Info*));

                    Request** buffer = (Request**)malloc(shared->bufferSize * sizeof(Request*));
                    for (int ii = 0; ii < shared->bufferSize; ii++) {
                        buffer[ii] = (Request*)malloc(sizeof(Request));
                    }
                    shared->output = output; 
                    shared->buffer = buffer;
                    shared->bufferLock = bufferLock;
                    shared->cond = cond;
                    numLines = shared->remaining; //The initial value of remaining represents the no. of lines in input

                    //Creating the threads
                    pthread_create(&id[0], NULL, liftR, (void*)shared);
                    for (int jj = 1; jj <= 3; jj++) {
                        info[jj - 1] = (Info*)malloc(sizeof(Info));
                        info[jj - 1]->liftNo = jj;
                        info[jj - 1]->shared = shared;
                        pthread_create(&id[jj], NULL, lift, (void*)info[jj - 1]);
                    }

                    //Waiting for the threads to finish executing
                    for (int kk = 0; kk < 4; kk++) {
                        pthread_join(id[kk], NULL); //Join so main waits until threads finish, start with id[0], liftR
                    }
                    fprintf(output, "Total number of requests: %d\n", numLines);
                    fprintf(output, "Total number of movements: %d", shared->combinedMovement);
                    printf("\nFinished simulation.\nOutput has been logged to 'sim_out.txt'.\n");

                    pthread_mutex_destroy(&bufferLock);
                    pthread_cond_destroy(&cond);
                    for (int ll = 0; ll < shared->bufferSize; ll++) {
                        free(shared->buffer[ll]);
                    }
                    free(buffer);
                    for (int mm = 0; mm < 3; mm++) {
                        free(info[mm]);
                    }
                    free(info);
                }
            }
            fclose(output);

        }
        free(shared);
    }
    else { //Error for running executable on command line incorrectly
        printf("Error: Program must be run as [lift_sim_A m t] where m is the buffer size and t is the time required "
               "per lift request\n");
    }

    return 0;
}

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
    else { //Command line parameters have been validated, hence values are inserted into settings struct
        shared->bufferSize = bufferSize;
        shared->requestTime = requestTime;
        shared->empty = shared->bufferSize; //Initialising empty as size of buffer, indicating that the buffer is empty
        shared->remaining = 0;
        shared->combinedMovement = 0;
        valid = 1;
    }
    return valid;
}