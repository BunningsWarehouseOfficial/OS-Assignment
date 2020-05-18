/* Filename:           request_handler_A.c
   Author:             Kristian Rados (19764285)
   Date Created:       19/04/2020
   Date Last Modified: 17/05/2020
   Purpose: This is where the requests used by the simulation are handled. The requests in the input file are first
            checked to make sure they are valid and then the liftR thread pulls requests out from the input file and
            adds them to the buffer when possible. */

#define _GNU_SOURCE //Fixes VSCODE issue where 'CLOCK_REALTIME' isn't recognised

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "lift_sim_A.h"
#include "request_handler_A.h"

/* PURPOSE: This is the producer thread function. It is the core of the simulation, as the whole simulation ends when
this function ends. Once it attains the mutex lock it pulls a request from the input file and adds it to the buffer. It
then logs this added request to the output log. */
void* liftR(void* arg) {
    FILE* input;
    Shared* shared = (Shared*)arg;
    
    input = fopen("sim_input.txt", "r"); //Opening the input file
    if (input == NULL) {
        perror("Error opening sim_input.txt\n");
    }
    else {
        FILE* output = shared->output;
        int numLines, index, timeout;
        Request** buffer = shared->buffer;
        pthread_mutex_t* bufferLock = &shared->bufferLock;
        pthread_cond_t* cond = &shared->cond;
        timeout = 0;

        numLines = shared->remaining; //The initial value of remaining represents the no. of lines in sim_input.txt
        printf("Running simulation...\n");

        //Producer loop
        for (int ii = 0; ii < numLines; ii++) {
            int source, destination;
            
            pthread_mutex_lock(bufferLock);
            while (shared->empty == 0 && timeout != ETIMEDOUT) {
                //Creating the absolute time that the thread will wait until before continuing
                struct timespec timeoutTime = {0, 0};
                clock_gettime(CLOCK_REALTIME, &timeoutTime);
                timeoutTime.tv_nsec += 500000000; //Timeout time of 0.5 seconds
                
                pthread_cond_timedwait(cond, bufferLock, &timeoutTime);
            }
            
        if (timeout != ETIMEDOUT) {
            index = shared->bufferSize - shared->empty;
            request(input, buffer[index]);
            source = buffer[index]->source;
            destination = buffer[index]->destination;
            shared->remaining--;
            shared->empty--; /* Requests are technically only added and removed from buffer in main(); in the threads
                                the values are overwritten and the empty value shifted to a different buffer position */

            #ifdef VERBOSE //Provides visual indication of simulation progress
            printf("Lift-R: #%d\n", ii + 1);
            #endif

            //Logging added request to sim_out.txt
            fprintf(output, "---------------------------------------------\n  ");
            fprintf(output, "New Lift Request From Floor %d to Floor %d\n  ", source, destination);
            fprintf(output, "Request No: %d\n", numLines - shared->remaining);
            fprintf(output, "---------------------------------------------\n\n");
            //End of critical section
        }
            pthread_cond_signal(cond);
            pthread_mutex_unlock(bufferLock);
        }
    }

    fclose(input);
    pthread_exit(NULL);
    return(NULL);
    //This is where the simulation unravels and ends, once all requests have been placed into the buffer
}

/* PURPOSE: This is the function used by liftR to pull one lift request at a time from the input file. It grabs a line
from the input file and parses it into the source and destination floor, which is placed in the Request that was passed
into the function as a parameter. */
void request(FILE* input, Request* request) {
    int line, source, destination;

    line = fscanf(input, "%d %d\n", &source, &destination);
    if (line == 2 && line != EOF) {
        request->source = source;
        request->destination = destination;
    }
    else {
        printf("Error: problem handling request\n");
    }
}

/* PURPOSE: This is the function called by main that checks that the input file is valid before starting the simulation.
If the input file is not deemed valid then an error message will be displayed and the program will end. This check prior
to beginning the simulation allows the request() function to pull requests one at a time without worrying about causing
and error and ending the simulation half way through. */
int checkInput(int* remaining) {
    FILE* f;
    int valid = 1; //'boolean'

    f = fopen("sim_input.txt", "r"); //Opening the input file
    if (f == NULL) {
        perror("Error opening sim_input.txt");
        valid = 0;
    }
    else {
        int line, a, b;

        line = fscanf(f, "%d %d\n", &a, &b);
        while (valid == 1 && line != EOF) {            
            if (line != EOF) {
                (*remaining)++; //Incrementing tally after fscanf() so as to not include 'empty' lines in the count

                if (line != 2) {
                    printf("Error: Format must be two integers separated by a space (sim_input.txt request %d)\n",
                           *remaining);
                    valid = 0;
                }
                else if (a > 20 || b > 20 || a < 1 || b < 1) {
                    printf("Error: Values must be between 1 and 20 (sim_input.txt request %d)\n", *remaining);
                    valid = 0;
                }
                else if (a == b) {
                    printf("Error: Source and destination floors can not be equal (sim_input.txt request %d)\n", 
                           *remaining);
                    valid = 0;
                }
            } 
            line = fscanf(f, "%d %d\n", &a, &b);
        }
        
        //Only checks if valid == 1 to avoid printing the error if it was likely caused by the loop being cut short
        if ((*remaining < 50 || *remaining > 100) && valid == 1) {
            printf("Error: Incorrect format in sim_input line, must be between 50 and 100 requests (lines)\n");
            valid = 0;
        }
        fclose(f);
    }
    return valid;
}