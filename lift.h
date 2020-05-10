#ifndef LIFT_H
#define LIFT_H

void* lift(void* arg);
void executeRequest(int* currentFloor, FILE* output, Info* info, Request* request);

#endif