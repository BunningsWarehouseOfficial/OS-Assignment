#define _GNU_SOURCE //Fixes VSCODE issue where 'CLOCK_REALTIME' isn't recognised

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include "lift_sim_B.h"
#include "lift_B.h"

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
    initial = shared->remaining; // ?

    while (shared->remaining > 0) {
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

void executeRequest(int* currentFloor, FILE* output, Info* info, Request* request) {
    Shared* shared = info->shared;
    int movement, source, destination;
    
    source = request->source;
    destination = request->destination;
    movement = abs(*currentFloor - source) + abs(source - destination);
    info->liftMovement += movement;
    shared->combinedMovement += movement;
    shared->index--;

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
    
    *currentFloor = destination;
}