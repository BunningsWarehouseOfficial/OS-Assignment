#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lift_sim_A.h"
#include "request_handler.h"

void* liftR(void* arg) {
    FILE* input;
    Shared* shared = (Shared*)arg;
    
    input = fopen("sim_input.txt", "r");
    if (input == NULL) {
        perror("Error opening sim_input.txt\n");
    }
    else {
        Request** buffer = shared->buffer;
        pthread_mutex_t* bufferLock = &shared->bufferLock;
        pthread_cond_t* cond = &shared->cond;
        int numLines = shared->remaining; //The initial value of remaining represents the no. of lines in sim_input.txt
        shared->empty = shared->bufferSize; //Initialising empty to indicate that the buffer is empty

        //Producer loop
        for (int ii = 0; ii < numLines; ii++) {
            //printf("R for loop ii=%d\n", ii); //
            pthread_mutex_lock(bufferLock);
            if (shared->empty == 0) {
                printf("R wait\n\n"); //
                pthread_cond_wait(cond, bufferLock);
            }

            //FIXME I shouldn't be mallocing a request every time I go into critical section
            //Critical section: Adding a request to the buffer
            printf("R Producing buffer[%d]\n", shared->bufferSize - shared->empty); //
            request(f, buffer[shared->bufferSize - shared->empty]);
            shared->empty--;
            shared->remaining--;
            printf("    R %d requests remaining\n", shared->remaining); // 
            pthread_cond_signal(cond);
            pthread_mutex_unlock(bufferLock);
            //End critical section
        }
    }

    fclose(input);
    pthread_exit(NULL);
    return(NULL);
    //TODO End the whole simulation/program here
}

//Lift-R - getting one request at a time
void request(FILE* input, Request* request) { //TODO make sure that 'valid' return is not required
    int line, source, destination;

    line = fscanf(input, "%d %d\n", &source, &destination);
    if (line == 2 && line != EOF) {
        request->source = source;
        request->destination = destination;
    }
    else {
        printf("Error: problem handling request\n"); //TODO remove this and add assertion if 'valid' is NEVER required
    }
}

int checkInput(int* remaining) {
    FILE* f;
    int valid = 1; //'boolean'

    f = fopen("sim_input.txt", "r");
    if (f == NULL) {
        perror("Error opening sim_input.txt");
    }
    else {
        int line, a, b;

        line = fscanf(f, "%d %d\n", &a, &b);
        while (valid == 1 && line != EOF) {            
            if (line != EOF) {
                (*remaining)++; //Tallying after fscanf() so as to not include 'empty' EOF lines in count

                if (line != 2) {
                    if (line == EOF) { printf("EOF!\n"); }
                    printf("Error: Format must be two numbers separated by a space (sim_input.txt line %d)\n", *remaining);
                    valid = 0;
                }
                else if (a > 20 || b > 20 || a < 1 || b < 1) {
                    printf("Error: Values must be between 1 and 20 (sim_input.txt line %d)\n", *remaining);
                    valid = 0;
                }
                else if (a == b) {
                    printf("Error: Source and destination floors can not be equal (sim_input.txt line %d)\n", *remaining);
                }
            } 
            line = fscanf(f, "%d %d\n", &a, &b);
        }

        if (*remaining < 50 || *remaining > 100) {
            printf("Error: Incorrect format in sim_input line, must be between 50 and 100 requests (lines)\n");
            valid = 0;
        }
        else {
            valid = 1;
        }
    }
    
    fclose(f);
    return valid;
}