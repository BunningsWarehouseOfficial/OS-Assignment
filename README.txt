Project: Lift Simulator (OS Sem. 1 2020 Assignment)
Author: Kristian Rados (19764285)
Date Created: 19/04/2020
Date Last Modified: 17/05/2020

--------------------------------------------------

COMPILING
To compile the program, Make must be utilised. To compile both versions of the simulation (lift_sim_A and lift_sim_B)
simply type 'make' into the terminal when in the directory.

To compile just one of the simulation versions type 'make lift_sim_A' or 'make lift_sim_B' into the terminal to compile
just the A simulation or just the B simulation respectively.

To compile both versions of the simulation with the VERBOSE conditional compilation option, type 'make VERBOSE=1' into
the terminal. If VERBOSE is chosen, the simulation will print to the terminal whenever a request is added to the
buffer or whenever a request is executed by a lift.

RUNNING
To run the simulation, type './lift_sim_A [m] [t]' into the console, where [m] is the buffer size and [t] is the time
in seconds taken to process each lift request. To switch between the A and B simulations, simply substitute the A for a
B and vice versa.

[m] should be an integer > 0, [t] should be an integer >= 0.