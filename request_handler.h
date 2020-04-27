#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

void* liftR(void* arg);
int request(FILE* f, Request* request);
int checkInput(int* lineNum);

#endif