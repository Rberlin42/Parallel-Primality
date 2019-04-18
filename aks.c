#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aks.h"


int is_whole(double x){
	return (floor(x) == ceil(x));
}

int aks_prime(int testval){
	// make sure the input is valid
	if(testval <= 1){
		fprintf(stderr, "input number is not valid, must be strictly greater than 1\n");
		return 0;
	}
	// check if the test value is a perfect power
	// if so, it will be composite.
	// if not, then we will keep testing it further
	double a;
	for(int b = 2; b <= log2(testval); b++){
		a = pow(testval, 1.0/b);

		if(is_whole(a)){
			return 0;
		}
		
	}


	return 1;
}