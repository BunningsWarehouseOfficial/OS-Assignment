#ifndef LIFT_SIM_B_H
#define LIFT_SIM_B_H

//Represents a single lift request from sim_input.txt
typedef struct {
    int source; //Represents the floor where the request was made
    int destination; //Represents the destination floor
} Request;
 
 //Struct that contains all the data that needs to be shared between the threads
typedef struct {
    int bufferSize; //The size of the request buffer; specified as command line argument
    int requestTime; //The time taken to execute each request; specified as command line argument
    int remaining; //The number of requests from sim_input.txt that are remaining to be processed
    int combinedMovement; //The total movement of all the lifts combined
    int index; //The index of the first empty spot in the buffer
    FILE* output; //FILE pointer for logging all activities to sim_out.txt
    sem_t mutex; //Semaphore to provide mutex to buffer and file writing
    sem_t full; //Semaphore representing how full the buffer is
    sem_t empty; //Semaphore representing how empty the buffer is
    Request** buffer; //Buffer containing produced requests that have yet to be consumed
} Shared;

//Wrapper struct for bringing Shared struct into lift threads while also managing extra information needed for lifts
typedef struct { 
    int liftNo; //Used for assigning the lift threads their respective numbers between 1 and 3
    int executed; //The number of requests that this lift has executed
    int liftMovement; //The total movement for this lift
    Shared* shared; //Pointer to buffer struct to retrieve shared information
} Info;

int loadSettings(Shared* shared, char* a, char* b);
void* sharedMMap(size_t size);

#endif