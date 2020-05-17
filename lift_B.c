/* Filename:           lift_B.c
   Author:             Kristian Rados (19764285)
   Date Created:       09/05/2020
   Date Last Modified: 17/05/2020
   Purpose: This is where the simulation executes the lift requests. The lift() process takes a request out from the
            buffer when possible and then processes the operations of that lift request. */

#define _GNU_SOURCE //Fixes VSCODE issue where 'CLOCK_REALTIME' isn't recognised

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include "lift_sim_B.h"
#include "lift_B.h"

/* PURPOSE: This is the consumer process function. Once it attains the mutex lock it pulls a request from the buffer and
executes that request. The process will then sleep for the time specified by the command line parameter t before
attempting to pull and execute another request. */
void lift(void* arg) {
    int currentFloor, initial;
    Info* info = (Info*)arg;
    Shared* shared = info->shared;
    Request** buffer = shared->buffer;
    sem_t* mutex = &shared->mutex;
    sem_t* full = &shared->full;
    sem_t* empty = &shared->empty;
    
    info->executed = 0;
    info->liftMovement = 0;
    currentFloor = 1;
    initial = shared->remaining;

    while (shared->remaining > 0) {
        //Creating the absolute time that the thread will wait until before continuing
        struct timespec timeoutTime = {0, 0};
        clock_gettime(CLOCK_REALTIME, &timeoutTime);
        timeoutTime.tv_nsec += 500000000; //Timeout time of 0.5 seconds

        //The initial case: no timeouts so as to prevent deadlock occurring at the beginning due to a slow producer
        if (shared->remaining == initial) {
            sem_wait(full);
            sem_wait(mutex);
        }
        else { //Standard waits: they timeout to prevent the final lift process becoming stuck in deadlock
            sem_timedwait(full, &timeoutTime);
            sem_timedwait(mutex, &timeoutTime);
        }

        //Critical section: Taking a request out of the buffer
        executeRequest(&currentFloor, shared->output, info, buffer[shared->index - 1]);
        //End of critical section

        sem_post(mutex);
        sem_post(empty);

        sleep(shared->requestTime);
    }
    exit(0);
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
    shared->index--; //shared->index no longer represents the first empty spot in the buffer; must be decremented

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
    fflush(output);
    
    *currentFloor = destination; //Updating which floor the lift is currently on
}