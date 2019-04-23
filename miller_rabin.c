#include <stdlib.h>
#include <stdio.h>
#include<mpi.h>

#define BGQ 1 // when running BG/Q, comment out when testing on mastiff

#ifdef BGQ
#include<hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

#define t 10
#define N 1073741824

double g_time_in_secs = 0;
double g_processor_frequency = 1600000000.0; // processing speed for BG/Q
#ifdef BGQ
    unsigned long long g_start_cycles=0;
    unsigned long long g_end_cycles=0;
#else
    double g_start_cycles;
    double g_end_cycles;
#endif

int mpi_myrank;
int mpi_commsize;

// modular exponentiation
// base^exp % mod
unsigned long long int power(unsigned long long int base, unsigned long long int exp, unsigned long long int mod){
    if(exp == 0)
        return 1;

    unsigned long long int z = power(base, exp/2, mod);

    //utilize following property to compute large numbers:
    // (a*b) mod n = ((a mod n) * (b mod n)) mod n
    if(exp % 2 == 0)
        // z^2 % mod
        return ((z % mod) * (z % mod)) % mod;
    else
        // (base * z^2) % mod
        return ((base % mod) * (((z % mod) * (z % mod)) % mod)) % mod;
}

//Miller Rabin implementation to check if a number n is prime
//true if probably prime
//false if composite
int miller_rabin(int n){
    if(n <= 3) return 1;

	// Find integers k, q, k > 0, q odd, so that (n–1)=2^k * q
	int q = n - 1; 
	int k = 0;

    while (q % 2 == 0) {
    	k++;
        q /= 2;
    }

    //pick a random number a, 1<a<n–1
    int a = (rand() % (n-2)) + 1;
 
    unsigned long long int x = power(a, q, n);
    if(x == 1 || x == n-1)
    	return 1;

   	for (int i = 0; i < k; i++){
    	x = power(x, 2, n);
    	q *= 2;

    	if(x == 1)
    		return 0;
    	if(x == n-1)
    		return 1;
    }

    return 0;
}

// counts the number of primes from 1 to n
int computePrimes(long n){
    long count = 0;

    for(long i = mpi_myrank+1; i < n; i += mpi_commsize){
        //run the test for t times
        int prime = 1;
        for(int j = 0; j < t; j++){
            if(!miller_rabin(i)){
                //if it ever returns composite, then it is definitely not prime
                prime = 0;
                break;
            }
        }

        count += prime;
    }

    return count;
}

int main(int argc, char* argv[]){

    MPI_Init( &argc, &argv);
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_commsize);
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_myrank);

    // begin timer
    if(mpi_myrank == 0)
        g_start_cycles = GetTimeBase();

    long num_primes = computePrimes(N);
    long total;
    MPI_Reduce(&num_primes, &total, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if(mpi_myrank == 0){
        g_end_cycles = GetTimeBase();
        #ifdef BGQ
            g_time_in_secs = ((double)(g_end_cycles - g_start_cycles)) / g_processor_frequency;
        #else
            g_time_in_secs = (g_end_cycles - g_start_cycles);
        #endif
        
        printf("Num Primes: %ld\n", total);
        printf("TIME: %f\n", g_time_in_secs);
    }

    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();

    return EXIT_SUCCESS;
}