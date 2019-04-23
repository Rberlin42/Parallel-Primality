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
ull upTo = 1073741824;
ull factors = 0;
ull totPrimes = 0;

//attempt division for each number in range of lower to upper
void trialDivision(ull lower, ull upper, ull prime) {
  for(ull i = lower + 1; i <= upper; ++i) {
    if(prime % i == 0) {
      ++factors;
    }
  }
}

int main(int argc, char** argv) {

  int mpi_myrank;
  int mpi_commsize;
  ull loopTo;
  ull offset;

  MPI_Init( &argc, &argv);
  MPI_Comm_size( MPI_COMM_WORLD, &mpi_commsize);
  MPI_Comm_rank( MPI_COMM_WORLD, &mpi_myrank);

  for(ull i = 1; i <= upTo; ++i) {
    loopTo = (ull) sqrt(i);
    offset = loopTo / mpi_commsize;

    if(mpi_myrank == 0)
      g_start_cycles = GetTimeBase();

    if(mpi_myrank == mpi_commsize - 1) {
      trialDivision(offset * mpi_myrank + 1, loopTo, i);
    } else {
        trialDivision(offset * mpi_myrank + 1, offset * mpi_myrank + offset, i);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if(mpi_myrank == 0) {
      if(factors == 0) {
        ++totPrimes;
      } 
    }
    factors = 0;
  }

  if(mpi_myrank == 0) {
    printf("%llu\n", totPrimes);
    g_end_cycles = GetTimeBase();
    #ifdef BGQ
        g_time_in_secs = ((double)(g_end_cycles - g_start_cycles)) / g_processor_frequency;
    #else
        g_time_in_secs = (g_end_cycles - g_start_cycles);
    #endif
    printf("TIME: %f\n", g_time_in_secs);
  }

}
