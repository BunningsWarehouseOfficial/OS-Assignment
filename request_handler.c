#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lift_sim_A.h"
#include "request_handler.h"

void* liftR(void* arg) {
    FILE* f;
    //int in, out;
    Info* info = (Info*)arg;
    //in = out = 0;
    
    f = fopen("sim_input.txt", "r");
    if (f == NULL) {
        perror("Error opening sim_input.txt");
    }
    else {
        int lineNum, valid; //'boolean'
        Request** buffer = info->buffer;
        valid = 1;
        lineNum = info->lineNum;
        printf("lineNum = %d\n", lineNum); //

        for (int ii = 0; ii < lineNum; ii++) {
            if (valid == 1) {
                //pthread_mutex_lock(&empty, NULL); //wait(empty) +
                //printf("Empty has locked");
                //pthread_mutex_lock(&mutex, NULL); //wait(mutex) +
                //printf("Mutex has locked\n");

                valid = request(f, buffer[ii]);
                printf("line %d\n", ii + 1); //

                //pthread_mutex_unlock(&mutex,NULL); //signal(mutex) +
                //printf("Mutex has unlocked\n");
                //pthread_mutex_unlock(&full, NULL); //signal(full) +
                //printf("Full has unlocked");
            }
        }
    }

    fclose(f);
    pthread_exit(NULL);
    return(NULL);
    //TODO End the whole simulation/program here
}

//Lift-R - getting one request at a time
int request(FILE* f, Request* request) {
    int line, source, destination, valid = 0; //'boolean'

    line = fscanf(f, "%d %d\n", &source, &destination);
    if (line == 2 && line != EOF) {
        request = (Request*)malloc(sizeof(Request));
        request->source = source;
        request->destination = destination;
        valid = 1;
    }

    return valid;    
}

int checkInput(int* lineNum) {
    FILE* f;
    int valid = 1; //'boolean'

    f = fopen("sim_input.txt", "r");
    if (f == NULL) {
        perror("Error opening sim_input.txt");
    }
    else {
        int line, a, b;

        while (valid == 1 && line != EOF) {
            line = fscanf(f, "%d %d\n", &a, &b);
            
            if (line != EOF) {
                (*lineNum)++; //Tallying after fscanf() so as to not include 'empty' EOF lines in count

                if (line != 2) {
                    if (line == EOF) { printf("EOF!\n"); }
                    printf("Error: Format must be two numbers separated by a space (sim_input.txt line %d)\n", *lineNum);
                    valid = 0;
                }
                else if (a > 20 || b > 20 || a < 1 || b < 1) {
                    printf("Error: Values must be between 1 and 20 (sim_input.txt line %d)\n", *lineNum);
                    valid = 0;
                }
                else if (a == b) {
                    printf("Error: Source and destination floors can not be equal (sim_input.txt line %d)\n", *lineNum);
                }
            }
        }

        if (*lineNum < 50 || *lineNum > 100) {
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