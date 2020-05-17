/* Filename:           lift_A.c
   Author:             Kristian Rados (19764285)
   Date Created:       19/04/2020
   Date Last Modified: 17/05/2020
   Purpose: This is where the simulation executes the lift requests. The lift() thread takes a request out from the
            buffer when possible and then processes the operations of that lift request. */

#define _GNU_SOURCE //Fixes VSCODE issue where 'CLOCK_REALTIME' isn't recognised

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "lift_sim_A.h"
#include "lift_A.h"

/* PURPOSE: This is the consumer thread function. Once it attains the mutex lock it pulls a request from the buffer and
executes that request. The thread will then sleep for the time specified by the command line parameter t before
attempting to pull and execute another request. */
void* lift(void* arg) {
    int currentFloor, consuming, timeout;
    Info* info = (Info*)arg;
    Shared* shared = info->shared;
    Request** buffer = shared->buffer;
    pthread_mutex_t* bufferLock = &shared->bufferLock;
    pthread_cond_t* cond = &shared->cond;
    timeout = 0;
    
    info->executed = 0;
    info->liftMovement = 0;
    currentFloor = 1;

    while (shared->remaining > 0) {
        pthread_mutex_lock(bufferLock);
        while (shared->empty == shared->bufferSize && timeout != ETIMEDOUT) {
            //Creating the absolute time that the thread will wait until before continuing
            struct timespec timeoutTime = {0, 0};
            clock_gettime(CLOCK_REALTIME, &timeoutTime);
            timeoutTime.tv_nsec += 500000000; //Timeout time of 0.5 seconds
            
            //A wait that times out to prevent the final lift thread becoming stuck in deadlock
            timeout = pthread_cond_timedwait(cond, bufferLock, &timeoutTime);
        }

        if (timeout != ETIMEDOUT) {
            //Critical section: Taking a request out of the buffer
            consuming = (shared->bufferSize - 1) - shared->empty; //Calculating the index of the request that's consumed
            executeRequest(&currentFloor, shared->output, info, buffer[consuming]);
            shared->empty++; //shared->empty no longer represents the no. of empty spots in buffer; must be incremented
            //End of critical section

            pthread_cond_signal(cond);
            pthread_mutex_unlock(bufferLock);
            sleep(shared->requestTime);
        }
        else { //Needed in the case that two lifs timeout to prevent an infinite cond wait without an extra sleep()
            pthread_cond_signal(cond);
            pthread_mutex_unlock(bufferLock);
        }
    }

    pthread_exit(NULL);
    return NULL;
}

/* PURPOSE: This is the function used by lift to execute the request that was taken from the buffer. It calculates the 
movement made and adds it to the thread's and the simulation's totals. It then logs this executed request to the output
log. */
void executeRequest(int* currentFloor, FILE* output, Info* info, Request* request) {
    Shared* shared = info->shared;
    int movement, source, destination;
    
    source = request->source;
    destination = request->destination;
    movement = abs(*currentFloor - source) + abs(source - destination);
    info->liftMovement += movement;       //Updating lift movement total
    shared->combinedMovement += movement; //Updating simulation movement total

    #ifdef VERBOSE //Provides visual indication of simulation progress
    printf("Lift-%d: %d -> %d -> %d\n", info->liftNo, *currentFloor, request->source, request->destination);
    #endif
    
    //Logging executed request to sim_out.txt
    fprintf(output, "Lift-%d Operation\n", info->liftNo);
    fprintf(output, "Previous position: Floor %d\n", *currentFloor);
    fprintf(output, "Request: Floor %d to Floor %d\n", source, destination);
    fprintf(output, "Detail operations:\n    ");
    fprintf(output, "Go from Floor %d to Floor %d\n    ", *currentFloor, source); //current -> source
    fprintf(output, "Go from Floor %d to Floor %d\n    ", source, destination); //source -> destination
    fprintf(output, "#movement for this request: %d\n    ", movement);
    fprintf(output, "#request: %d\n    ", ++info->executed); //The number of requests this lift has executed
    fprintf(output, "Total #movement: %d\n", info->liftMovement); //Total movement of this lift
    fprintf(output, "Current position: Floor %d\n\n", destination);
    
    *currentFloor = destination; //Updating which floor the lift is currently on
}