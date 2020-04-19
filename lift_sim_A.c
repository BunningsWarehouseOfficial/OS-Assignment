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

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* threadFn() { //Doesn't work unless before main() ? +
    sleep(1);
    printf("Hello, the thread has executed\n");
    return NULL; //Needs to return a void* for some reason +
}

int main(int argc, char* argv[]) {
    if (argc == 3) {
        Settings* settings = (Settings*)malloc(sizeof(Settings));
        
        if (loadSettings(Settings* settings, argv[1], argv[2]) = 1) {
            pthread_t lift1, lift2, lift3;

        }



        free(settings);
    }
    else { //Error for running executable on command line incorrectly
        printf("Error: Program must be run as [lift_sim_A m t] where m is the buffer size and t is the time required " +
               "per lift request\n");
    }

    return 0;
}

int loadSettings(Settings* settings, bufferSize, requestTime) {
    int scan1, scan2, bufferSize, requestTime, valid;
    valid = 0;
    sscanf(argv[1], "%d", &bufferSize);
    sscanf(argv[2], "%d", &requestTime);

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
        settings->bufferSize = bufferSize;
        settings->requestTime = requestTime;
        valid = 1;
    }
    return valid;
}

    //Geeks for Geeks example, redundant +
    //printf("About to create a thread\n");                      //*thread_id passed below is set by the create function
    //pthread_create(&thread_id, NULL, threadFn, NULL); //pthread_create(*thread_id, attributes, function, <-args)
    //pthread_join(thread_id, NULL); //Equivalent of wait() for processes, waits until specified thread terminates
    //printf("Thread has been created\n");