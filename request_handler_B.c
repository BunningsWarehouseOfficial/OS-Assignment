#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "lift_sim_B.h"
#include "request_handler_B.h"

void liftR(void* arg) {
    FILE* input;
    Shared* shared = (Shared*)arg;
    
    input = fopen("sim_input.txt", "r");
    if (input == NULL) {
        perror("Error opening sim_input.txt\n");
    }
    else {
        FILE* output = shared->output;
        int numLines;
        Request** buffer = shared->buffer;
        sem_t* mutex = &shared->mutex;
        sem_t* full = &shared->full;
        sem_t* empty = &shared->empty;
        
        numLines = shared->remaining; //The initial value of remaining represents the no. of lines in sim_input.txt
        printf("Running simulation...\n");

        //Producer loop
        for (int ii = 0; ii < numLines; ii++) {
            int source, destination, index;
            int testi = 0; //
            int* test = &testi; //
            
            sem_getvalue(empty, test); //
            printf("R wait empty %d--\n", *test); //
            sem_wait(empty);
            sem_getvalue(mutex, test); //
            printf("R wait mutex %d--\n", *test); //
            sem_wait(mutex);

            //Critical section: Adding a request to the buffer
            index = shared->index;
            request(input, buffer[index]);
            source = buffer[index]->source; //Lifts may change this value, hence need to retrieve while mutex is locked
            destination = buffer[index]->destination; //As above
            shared->remaining--;
            shared->index++;
        
            //#ifdef VERBOSE
            printf("Lift-R: #%d\n", ii + 1);
            //#endif

            //Logging added request to sim_out.txt
            fprintf(output, "---------------------------------------------\n  ");
            fprintf(output, "New Lift Request From Floor %d to Floor %d\n  ", source, destination);
            fprintf(output, "Request No: %d\n", numLines - shared->remaining);
            fprintf(output, "---------------------------------------------\n\n");
            //End of critical section

            sem_getvalue(mutex, test); //
            printf("R post mutex %d++\n", *test); //
            sem_post(mutex);
            sem_getvalue(full, test); //
            printf("R post full %d++\n", *test); //
            sem_post(full);
        }
    }

    fclose(input);
    exit(0);
    //This is where the program unravels and ends, once all requests have been placed into the buffer
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
        printf("Error: problem handling request\n");
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