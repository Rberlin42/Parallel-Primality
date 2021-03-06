#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>
#include "aks.h"

#define BGQ 1 // when running BG/Q, comment out when testing on mastiff

#ifdef BGQ
#include<hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

#define strong_scaling 0

double g_time_in_secs = 0;
double g_processor_frequency = 1600000000.0; // processing speed for BG/Q
#ifdef BGQ
    unsigned long long g_start_cycles=0;
    unsigned long long g_end_cycles=0;
#else
    double g_start_cycles;
    double g_end_cycles;
#endif


// modular exponentiation
// base^exp % mod
unsigned long long int mod_power(unsigned long long int base, unsigned long long int exp, unsigned long long int mod){
    if(exp == 0)
        return 1;

    unsigned long long int z = mod_power(base, exp/2, mod);

    //utilize following property to compute large numbers:
    // (a*b) mod n = ((a mod n) * (b mod n)) mod n
    if(exp % 2 == 0)
        // z^2 % mod
        return ((z % mod) * (z % mod)) % mod;
    else
        // (base * z^2) % mod
        return ((base % mod) * (((z % mod) * (z % mod)) % mod)) % mod;
}


int phi(unsigned int n){ 
    unsigned int result = 1; 
    for (int i = 2; i < n; i++) 
        if (GCD(i, n) == 1) 
            result++; 
    return result; 
} 


int ord(int a, int n){
    if (GCD(a, n) != 1) 
    	return -1; 

    // result keeps track of the last modular calculation
    // rather than calculating every power we can just multiply 
	//    	a by the last result and take the modulo n
    unsigned int result = 1; 
  
    int k = 1; 
    while (k < n){  
        result = (result * a) % n; 
 
        if (result == 1) 
            return k; 

        k++; 
    } 
  
    return -1; 
}


int GCD(int a, int b) 
{ 
    if (b == 0){ 
    	return a; 
    }
    else{
      	return GCD(b, a % b); 
    }
} 


int is_whole(double x){
	return (floor(x) == ceil(x));
}


int aks_prime(int testval){
	// make sure the input is valid
	if(testval <= 1){
		fprintf(stderr, "input number is not valid, must be strictly greater than 1\n");
		return 0;
	}
	else{
		//printf("Input number %d is a valid input\n", testval);
	}
	// check if the test value is a perfect power
	// if so, it will be composite.
	// if not, then we will keep testing it further
	//printf("Testing if %d is a perfect power\n", testval);
	double base;
	for(int b = 2; b <= log2(testval); b++){
		base = pow(testval, 1.0/b);

		if(is_whole(base)){
			//printf("Input %d is a perfect power (%d^%d), therefore it is composite.\n", testval, (int)base, b);
			
			// return composite
			return 0;
		}

	}

	//printf("Input %d is not a perfect power, now finding r value\n", testval);


	// find the smallest r such that ord_r(testval) > (log2(testval))^2
	// 		and r and testval are coprime
	int r;
	int min_ord = (int) pow( log2(testval), 2 );

	int maxr = 3;
	if(ceil(pow(log2(testval), 5) ) > 3){

		maxr = (int) ceil( pow( log2(testval), 5 ) );

	}

	// tells us when to stop counting
	int nextR = 1;
	// increment r by one each time
	for(r = 2; nextR && r < maxr; r++){
		// make sure the multiplicative order is greater than the log squared
		// (ord also checks if testval and r are coprime,and returns -1 if they aren't)
		if(ord(testval, r) > min_ord){
			// use this r value
			nextR = 0;
			break;
		}
		else{
			// keep looking for r
			nextR = 1;
		}

	}

	//printf("Found r value %d for the input value %d\n", r, testval);

	// a is the minimum of r and n-1
	int a = r;
	if(testval-1 < r){
		a = testval-1;
	}

	for( ; a > 1; a--){
		
		int g = GCD(testval, a);
		if(g > 1 &&  g < testval){
			//printf("Input value %d shares GCD %d with value %d and 2 <= %d <= min(r, testval-1), so %d is composite\n", testval, g, a, a, testval);

			// return composite
			return 0;
		}

	}

	//printf("Did not find value a where 2 <= a <= min(r, testval-1) that divides input value %d\n", testval);

	if(testval <= r){
		//printf("Input value %d is less than or equal to r (%d), therefore %d is prime\n", testval, r, testval);

		// return prime
		return 1;
	}

	//printf("Input value %d is not less than or equal to r value of %d\n", testval, r);	

	//printf("Testing input value %d for the polynomial remainder property\n", testval);
	int polymax = (int) floor(sqrt(phi(r)) * log2(testval));

	for(int c = 1; c <= polymax; c++){

		long double polymod = mod_power(c, testval, testval) - c;
		
		
		if(polymod != 0){
			//printf("Found a constant c = %d where the polynomial remainder property does not satisfy, therefore test value %d is composite\n", c, testval);
			// return composite
			return 0;
		}

	}

	//printf("Input value %d satisfied all tests, therefore it is prime\n", testval);
	// return prime
	return 1;
}


int main(int argc, char* argv[]){
	int mpi_rank = -1;
	int mpi_size = -1;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	// for weak scaling, the power of 2 goes up by 1 every time we double the ranks
	int nodes = mpi_size / 64;
	int p2 = 15 + log2(nodes);

	int input_size;
	if (strong_scaling){
		input_size = pow(2, 22);
	}
	else{
		input_size = pow(2, p2);
	}

	if(mpi_rank == 0){
		printf("Begin MPI prime counter with %d ranks, and a limit of %d\n", mpi_size, input_size);
	}

	int num_primes = 0;
	
	MPI_Barrier(MPI_COMM_WORLD);

	// calculate starting time
    if(mpi_rank == 0){
        g_start_cycles = GetTimeBase();
    }


    int isprime;
	for(int testval = 2 + mpi_rank; testval <= input_size; testval += mpi_size){
		isprime = aks_prime(testval);
		if(isprime){
			num_primes++; 
		}	
	}

	// wait for all the ranks to finish. Once done we will combine the results
	MPI_Barrier(MPI_COMM_WORLD);
	
	int* total_primes;

	if(mpi_rank == 0){
		total_primes = calloc(1, sizeof(int));
		MPI_Reduce(
			&num_primes,
			total_primes,
			1,
			MPI_INT,
			MPI_SUM,
			0,
			MPI_COMM_WORLD
		);
	}
	else{
		MPI_Reduce(
			&num_primes,
			NULL,
			1,
			MPI_INT,
			MPI_SUM,
			0,
			MPI_COMM_WORLD
		);
	}

	// MPI_Barrier(MPI_COMM_WORLD);
	// calculate ending time
	if(mpi_rank == 0){
        g_end_cycles = GetTimeBase();
        #ifdef BGQ
            g_time_in_secs = ((double)(g_end_cycles - g_start_cycles)) / g_processor_frequency;
        #else
            g_time_in_secs = (g_end_cycles - g_start_cycles);
        #endif
        
        printf("Num Ranks: %d\n", mpi_size);
        printf("Num Primes: %d\n", *total_primes);
        printf("TIME: %f\n", g_time_in_secs);
        free(total_primes);
    }

	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;

}