#ifndef LIFT_SIM_A_H
#define LIFT_SIM_A_H

typedef struct {
    int bufferSize;
    int requestTime;
} Settings;

typedef struct {
    int source;
    int destination;
} Request;

typedef struct { //Merge with settings? +
    pthread_mutex_t mutex;
    pthread_mutex_t full;
    pthread_mutex_t empty;
    Request** buffer;
} Info;

int loadSettings(Settings* settings, char* a, char* b);

#endif