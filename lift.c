#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lift.h"
#include "lift_sim_A.h"

void* lift(void* arg) {
    int lNum = *(int*)arg;
    
    printf("Hi, this is lift #%d\n", lNum);
    
    pthread_exit(NULL);
    return NULL;
}