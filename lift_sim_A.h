#ifndef LIFT_SIM_A_H
#define LIFT_SIM_A_H

typedef struct {
    int source;
    int destination;
} Request;

typedef struct { //TODO Add comment for each field
    int bufferSize;
    int requestTime;
    int remaining; //The number of requests from sim_input.txt that are remaining to be processed
    int currentLift; //Used for assigning the lift threads their number between 1 and 3
    int empty; //Number of empty slots in buffer
    pthread_mutex_t bufferLock;
    pthread_cond_t cond;
    Request** buffer;
} Info;

int loadSettings(Info* info, char* a, char* b);

#endif