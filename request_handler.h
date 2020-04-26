#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

void* liftR(Request** buffer, int bufferSize, int lineNum);
int request(FILE* f, Request* request);
int checkInput(int* lineNum);

#endif