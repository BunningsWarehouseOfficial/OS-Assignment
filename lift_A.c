#define _GNU_SOURCE //Fixes VSCODE issue where 'CLOCK_REALTIME' isn't recognised

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "lift_sim_A.h"
#include "lift_A.h"

void* lift(void* arg) {
    int currentFloor, consuming, timeout;
    Info* info = (Info*)arg;
    Shared* shared = info->shared;
    Request** buffer = shared->buffer;
    pthread_mutex_t* bufferLock = &shared->bufferLock;
    pthread_cond_t* cond = &shared->cond;
    
    info->executed = 0;
    info->liftMovement = 0;
    currentFloor = 1;

    while (shared->remaining > 0) {
        pthread_mutex_lock(bufferLock);
        while (shared->empty == shared->bufferSize && timeout != ETIMEDOUT) {
            struct timespec timeoutTime = {0, 0};
            clock_gettime(CLOCK_REALTIME, &timeoutTime);
            timeoutTime.tv_nsec += 200000000; //Timeout time of 0.2 seconds
            
            timeout = pthread_cond_timedwait(cond, bufferLock, &timeoutTime);
        }

        if (timeout != ETIMEDOUT) {
            //Critical section: Taking a request out of the buffer
            consuming = (shared->bufferSize - 1) - shared->empty;
            executeRequest(&currentFloor, shared->output, info, buffer[consuming]);
            shared->empty++;
            pthread_cond_signal(cond);
            pthread_mutex_unlock(bufferLock);
            //End of critical section

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

void executeRequest(int* currentFloor, FILE* output, Info* info, Request* request) {
    Shared* shared = info->shared;
    int movement, source, destination;
    
    source = request->source;
    destination = request->destination;
    movement = abs(*currentFloor - source) + abs(source - destination);
    info->liftMovement += movement;
    shared->combinedMovement += movement;

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
    
    *currentFloor = destination;
}