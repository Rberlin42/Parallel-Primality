#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include "aks.h"

int main(int argc, char* argv[]){
	int mpi_rank = -1;
	int mpi_size = -1;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);


	int maxtest = 0;
	if(maxtest){
		int testval = 2;

		while(testval < powl(2,30)){
		
			int isprime = aks_prime(testval, mpi_size, mpi_rank, MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			if(mpi_rank == 0){
				if(isprime){
					printf("AKS test ran on %d, found to be prime\n", testval);
				}
				else{
					printf("AKS test ran on %d, found to be composite\n", testval);

				}
			}

			testval++;
		}
	}
	else{
		int testval = 31;
		int isprime = aks_prime(testval, mpi_size, mpi_rank, MPI_COMM_WORLD);
		if(mpi_rank == 0){
			if(isprime){
				printf("AKS test ran on %d, found to be prime\n", testval);
			}
			else{
				printf("AKS test ran on %d, found to be composite\n", testval);
			}
		}

	}
	return 0;
}