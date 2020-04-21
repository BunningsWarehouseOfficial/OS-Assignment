void liftR(int bufferSize) {
    FILE* f;
    Request** buffer = (Request**)malloc(bufferSize * sizeof(Request*));    
    
    f = fopen("sim_input.txt", "r");
    if (f == NULL) {
        perror("Error opening settings file");
    }
    else {
        request(f, request);
    }

    free(buffer);
}

//Lift-R - getting one request at a time
int request(FILE* f, Request* request) {
    int line, source, destination, valid = 0; //'boolean'

    line = fscanf(f, "%d %d\n", &source, &destination);

    fclose(f);
}

int checkInput() {
    FILE* f;
    int valid == 1; //'boolean'

    f = fopen("sim_input.txt", "r");
    if (f == NULL) {
        perror("Error opening settings file");
    }
    else {
        int line, a, b, lineNum;
        lineNum = 0;

        while (valid == 1 && line != EOF) {
            line = fscanf(f, "%d %d\n", a, b);
            if (line != 2) {
                printf("Error: Incorrect format in sim_input line, must be two numbers separated by a space");
                valid = 0;
            }
            else if (a > 20 || b > 20 || a < 1 || b < 1) {
                printf("Error: Incorrect format in sim_input line, values must be between 1 and 20");
                valid = 0;
            }
            lineNum++;
        }

        if (lineNum < 50 || lineNum > 100) {
            printf("Error: Incorrect format in sim_input line, must be between 50 and 100 requests (lines)");
            valid = 0;
        }
        else {
            valid = 1;
        }
    }
    
    fclose(f);
    return valid;
}