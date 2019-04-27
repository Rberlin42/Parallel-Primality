#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// #define BGQ 1 // when running BG/Q, comment out when testing on mastiff
#ifdef BGQ
#include<hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

double g_time_in_secs = 0;
double g_processor_frequency = 1600000000.0; // processing speed for BG/Q
#ifdef BGQ
    unsigned long long g_start_cycles=0;
    unsigned long long g_end_cycles=0;
#else
    double g_start_cycles;
    double g_end_cycles;
#endif

typedef unsigned long long ull;

int mpi_myrank;
int mpi_commsize;

//For adding up primes
ull globalPCount = 0;
ull localPCount = 0;

ull cutOff = 0;

//Trial Division
void trialDivision(ull p) {
    cutOff = (ull) sqrt(p); // cutoff for how many numbers below p need to be tested
    for(ull n = 2; n <= cutOff; ++n) {
        if(p % n == 0) { //If factor other than 1 and itself then return. For improved runtime
            return;
        }
    }
    //increment if prime
    ++localPCount;    
}

int main(int argc, char** argv) {

    //CHANGE THIS TO CHANGE HOW MANY NUMBERS TO TEST PRIMALITY FOR
    ull primes = 4409;

    MPI_Init( &argc, &argv);
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_commsize);
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_myrank);

    //Start the time
    if(mpi_myrank == 0) {
        g_start_cycles = GetTimeBase();
    } 

    //Round Robin to divide up primes to each rank
    for(ull p = mpi_myrank + 1; p < primes; p += mpi_commsize) {
        trialDivision(p);
    }

    //Add up all of the primes from each rank
    MPI_Reduce(&localPCount, &globalPCount, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    //Print primes and print time
    if(mpi_myrank == 0) {
        printf("%llu\n", globalPCount); 
        
        g_end_cycles = GetTimeBase();
        #ifdef BGQ
            g_time_in_secs = ((double)(g_end_cycles - g_start_cycles)) / g_processor_frequency;
        #else
            g_time_in_secs = (g_end_cycles - g_start_cycles);
        #endif
        printf("TIME: %f\n", g_time_in_secs);    
    }    

    MPI_Finalize();
    return 0;
}