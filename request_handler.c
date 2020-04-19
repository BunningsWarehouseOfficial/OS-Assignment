void anotheroneyeahyeahyeah(int bufferSize) { //TODO geeze, change this name please, I'm begging you
    FILE* f;
    Request** buffer = (Request**)malloc(bufferSize * sizeof(Request*));    
    f = fopen("sim_input.txt", "r");

    if (request(f, request) = 0) {
        printf("Error: Because sim_input has incorrectly formatted data, no more requests will be added to the buffer");
    }

    free(buffer);
}

//Lift-R - getting one request at a time
int request(FILE* f, Request* request) {
    int valid = 0; //'boolean'

    if (f == NULL) {
        perror("Error opening settings file");
    }
    else {
        int line, source, destination;

        line = fscanf(f, "%d %d\n", &source, &destination);
        if (line 

        fclose(f);
    }
}