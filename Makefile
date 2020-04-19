CC = gcc
CFLAGS = -pthread -Wall -Werror -std=c99 -g
OBJ1 = lift_sim_A.o
OBJ2 = lift_sim_B.o
EXEC1 = A
EXEC2 = B

ifdef DEBUG
CFLAGS += -D DEBUG
DEBUG : clean $(EXEC)
endif

all : $(EXEC1) $(EXEC2)

$(EXEC1) : $(OBJ1)
	$(CC) -pthread $(OBJ1) -o $(EXEC1)

$(EXEC2) : $(OBJ2)
	$(CC) $(OBJ2) -o $(EXEC2)

lift_sim_A.o : lift_sim_A.c lift_sim_A.h
	$(CC) -c lift_sim_A.c $(CFLAGS)

lift_sim_B.o : lift_sim_B.c lift_sim_B.h
	$(CC) -c lift_sim_B.c $(CFLAGS)

clean:
	rm -f $(EXEC1) $(EXEC2) $(OBJ1) $(OBJ2)