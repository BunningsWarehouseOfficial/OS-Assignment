#include <stdlib.h>
#include <stdio.h> //
#include <pthread.h>
#include <unistd.h>
#include "lift_sim_A.h"
#include "lift.h"

void* lift(void* arg) {
    int lNum, currentFloor;
    Info* info = (Info*)arg;
    Request** buffer = info->buffer;
    pthread_mutex_t* bufferLock = &info->bufferLock;
    pthread_cond_t* cond = &info->cond;
    lNum = info->currentLift;
    currentFloor = 1;

    printf("L%d entering while loop\n", lNum); //
    while (info->remaining > 0) {
        pthread_mutex_lock(bufferLock);
        if (info->empty == info->bufferSize) {
            printf("L%d wait\n", lNum); //
            pthread_cond_wait(cond, bufferLock);
        }

        //printf("L%d: ", lNum);
        //sleep(info->requestTime);
        executeRequest(&currentFloor, buffer[info->bufferSize - info->empty - 1]);
        info->empty++;
    }
    
    pthread_exit(NULL);
    return NULL;
}

void executeRequest(int* currentFloor, Request* request) {
    //BUG request is segfaulting
    printf("%d -> %d -> %d\n", *currentFloor, request->source, request->destination);
    //log

    //request == NULL;
}