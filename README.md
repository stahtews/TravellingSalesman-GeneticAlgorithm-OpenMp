# TravellingSalesman-GeneticAlgorithm-OpenMp

Parallelization Strategy

Travelling salesman problem using Genetic algorithm to find the semi optimal trip is implemented and parallelized on a Shared Memory System using OpemMP. The program is modularized to the maximum and consists of various methods which performs utmost one function.

OpenMp explanation and implementation

Parallelization for computer systems with shared memory (SM) means automatic distribution of loop partitions on multiple processors, or the explicit distribution of work on the processors with compiler directives and runtime function calls (Open MP). The Open MP API is defined for Fortran, C and C++, it comprises of compiler directives, runtime routines, and environment variables. At the beginning of the first parallel region of an Open MP program (these are the program parts enclosed in the parallel directive), several Threads are started. These threads execute the parallel region redundantly until they reach a so called Worksharing Construct ( ‘}’ symbol), in which the arising work is divided among the Threads.

According to Amdahl’s law, fraction of the code which can run in parallel( which can be assigned to a processor and can carry out the function independent of next or previous iteration operation) should be optimized. Only this fraction of the code has the power to influence the speed by increasing the number of processors and the serial code doesn’t have influence on efficiency improvement during parallelization whatsoever.

In the code specified below, Evaluate() method calculates the distance of each chromosome in the trip array. This calculation demands nested for loops and is private(independent) to each chromosome. Crossover() method spawns 2 offsprings using 2 chromosomes from the trip argument which also demands nested queries and dependent on 2 chromosomes at a time. Three methods findCharVal(), findPos() and findDistance() are the most used methods by evaluate() and crossover(). Two characteristics, computation-intensive and independence(less synchronization or less associativity) of methods makes it an eligible for parallelization.
