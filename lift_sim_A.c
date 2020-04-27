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

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "lift_sim_A.h"
#include "request_handler.h"
#include "lift.h"

int main(int argc, char* argv[]) {
    if (argc == 3) {
        Info* info = (Info*)malloc(sizeof(Info));

        printf("Loading settings\n"); //
        if (loadSettings(info, argv[1], argv[2]) == 1) {
            info->lineNum = 0;

            printf("Checking input\n"); //
            if (checkInput(&info->lineNum) == 1) {
                pthread_t id[4];
                //pthread_mutex_t mutex, full, empty;
                int liftArgs[3];

                //TODO for loop initialisation of buffer if necessary
                Request** buffer = (Request**)malloc(info->bufferSize * sizeof(Request*));
                //Request** buffer = (Request**)calloc(info->bufferSize, sizeof(Request*));

                info->buffer = buffer;
                //info->mutex = mutex;
                //info->full = full;
                //info->empty = empty;

                for (int jj = 0; jj < 3; jj++) {
                    liftArgs[jj] = jj + 1;
                    pthread_create(&id[jj], NULL, lift, &liftArgs[jj]);
                }
                pthread_create(&id[3], NULL, liftR, (void*)info);

                for (int kk = 0; kk < 4; kk++) {
                    pthread_join(id[kk], NULL);
                }

                //pthread_mutex_init(&mutex, NULL);
                //thread_mutex_init(&full, NULL);
                //pthread_mutex_init(&empty, NULL);

                //TODO destroy mutexes when finished

                free(buffer);
            }
        }

        free(info);
    }
    else { //Error for running executable on command line incorrectly
        printf("Error: Program must be run as [lift_sim_A m t] where m is the buffer size and t is the time required "
               "per lift request\n");
    }

    return 0;
}

int loadSettings(Info* info, char* a, char* b) {
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
        info->bufferSize = bufferSize;
        info->requestTime = requestTime;
        valid = 1;
    }
    return valid;
}

    //Geeks for Geeks example, redundant +
    //printf("About to create a thread\n");                      //*thread_id passed below is set by the create function
    //pthread_create(&thread_id, NULL, threadFn, NULL); //pthread_create(*thread_id, attributes, function, <-args)
    //pthread_join(thread_id, NULL); //Equivalent of wait() for processes, waits until specified thread terminates
    //printf("Thread has been created\n");