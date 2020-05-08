#include <stdlib.h>
#include <stdio.h> //
#include <pthread.h>
#include <unistd.h>
#include "lift_sim_A.h"
#include "lift.h"

void* lift(void* arg) {
    int liftNo, currentFloor, consuming;
    Info* info = (Info*)arg;
    Shared* shared = info->shared;
    Request** buffer = shared->buffer;
    pthread_mutex_t* bufferLock = &shared->bufferLock;
    pthread_cond_t* cond = &shared->cond;
    
    Request* request;
    currentFloor = 1;
    liftNo = info->liftNo;

    while (shared->remaining > 0) {
        pthread_mutex_lock(bufferLock);
        if (shared->empty == shared->bufferSize) {
            printf("L%d wait\n", liftNo); //
            pthread_cond_wait(cond, bufferLock);
        }

        //Critical section: Taking a request out of the buffer
        consuming = (shared->bufferSize - 1) - shared->empty;
        request = buffer[consuming];
        shared->empty++;
        pthread_cond_signal(cond);
        pthread_mutex_unlock(bufferLock);
        //End critical section

        printf("L%d Consumed request buffer[%d]\n", liftNo, consuming); //
        sleep(shared->requestTime);
        executeRequest(&currentFloor, request);
    }
    
    //BUG there was a random segfault right after two threads did their first consumption that doesn't always happen

    pthread_exit(NULL);
    return NULL;
}

void executeRequest(int* currentFloor, Request* request) {
    //BUG request is segfaulting
    printf("%d -> %d -> %d\n", *currentFloor, request->source, request->destination);
    *currentFloor = request->destination;
    
    //TODO separate into separate function if suitable
    //Logging
}