#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "aks.h"

int main(int argc, char* argv[]){
	int testval = 31;
	
	int isprime = aks_prime(testval);
	
	if(isprime){
		printf("AKS test ran on %d, found to be prime\n", testval);
	}
	else{
		printf("AKS test ran on %d, found to be composite\n", testval);
	}


	return 0;
}