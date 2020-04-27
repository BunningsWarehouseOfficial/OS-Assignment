#ifndef LIFT_SIM_A_H
#define LIFT_SIM_A_H

typedef struct {
    int source;
    int destination;
} Request;

typedef struct { //Merge with settings? +
    int bufferSize;
    int requestTime;
    int lineNum;
    pthread_mutex_t mutex;
    pthread_mutex_t full;
    pthread_mutex_t empty;
    Request** buffer;
} Info;

int loadSettings(Info* info, char* a, char* b);

#endif