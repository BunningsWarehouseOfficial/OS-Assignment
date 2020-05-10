CC = gcc
CFLAGS = -Wall -Werror -std=c99 -g
OBJ1 = lift_sim_A.o request_handler_A.o lift_A.o
OBJ2 = lift_sim_B.o request_handler_B.o lift_B.o
EXEC1 = lift_sim_A
EXEC2 = lift_sim_B

ifdef VERBOSE
CFLAGS += -D VERBOSE
VERBOSE : clean $(EXEC1) $(EXEC2)
endif

all : $(EXEC1) $(EXEC2)

$(EXEC1) : $(OBJ1)
	$(CC) -pthread $(OBJ1) -o $(EXEC1)

$(EXEC2) : $(OBJ2)
	$(CC) -pthread $(OBJ2) -o $(EXEC2)

lift_sim_A.o : lift_sim_A.c lift_sim_A.h request_handler_A.h lift_A.h
	$(CC) -c lift_sim_A.c $(CFLAGS)

request_handler_A.o : request_handler_A.c request_handler_A.h lift_sim_A.h
	$(CC) -c request_handler_A.c $(CFLAGS)

lift_A.o : lift_A.c lift_A.h
	$(CC) -c lift_A.c $(CFLAGS)

lift_sim_B.o : lift_sim_B.c lift_sim_B.h
	$(CC) -c lift_sim_B.c $(CFLAGS)

request_handler_B.o : request_handler_B.c request_handler_B.h lift_sim_B.h
	$(CC) -c request_handler_B.c $(CFLAGS)

lift_B.o : lift_B.c lift_B.h
	$(CC) -c lift_B.c $(CFLAGS)

clean:
	rm -f $(EXEC1) $(EXEC2) $(OBJ1) $(OBJ2)