#ifndef LIFT_SIM_A_H
#define LIFT_SIM_A_H

typedef struct {
    int source;
    int destination;
} Request;

typedef struct { //TODO elaborate on difference between Shared and Info
    int remaining; //The number of requests from sim_input.txt that are remaining to be processed
    int empty; //Number of empty slots in buffer
    int bufferSize;
    int requestTime;
    pthread_mutex_t bufferLock;
    pthread_cond_t cond;
    Request** buffer;
} Shared;

typedef struct { //TODO Add comment for each field
    int liftNo; //Used for assigning the lift threads their respective numbers between 1 and 3
    Shared* shared; //Pointer to buffer struct to retrieve shared information
} Info;

int loadSettings(Shared* shared, char* a, char* b);

#endif