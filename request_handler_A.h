#ifndef REQUEST_HANDLER_A_H
#define REQUEST_HANDLER_A_H

void* liftR(void* arg);
void request(FILE* f, Request* request);
int checkInput(int* lineNum);

#endif