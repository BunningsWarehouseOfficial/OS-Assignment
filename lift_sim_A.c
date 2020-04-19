/* Filename:           lift_sim_A.c
   Author:             Kristian Rados (19764285)
   Date Created:       19/04/2020
   Date Last Modified: 19/04/2020
   Purpose:                                                                                                           */

//TODO change back executable names to full names

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* threadFn() {
    sleep(1);
    printf("Hello, the thread has executed\n");
    return NULL;
}
int main() {
    pthread_t thread_id; //Integer used to identify the thread in the system

    printf("About to create a thread\n");                        //*thread_id passed below is set by the create function
    pthread_create(&thread_id, NULL, threadFn, NULL); //pthread_create(*thread_id, attributes, function, <-args)
    pthread_join(thread_id, NULL); //Equivalent of wait() for processes, waits until specified thread terminates
    printf("Thread has been created\n");

    return 0;
}
