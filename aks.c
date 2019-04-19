#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aks.h"


int phi(unsigned int n) 
{ 
    unsigned int result = 1; 
    for (int i = 2; i < n; i++) 
        if (gcd(i, n) == 1) 
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
    while (k < n) 
    {  
        result = (result * a) % n ; 
 
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
      	return GCD(b, a % b) ; 
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
		printf("Input number %d is a valid input\n", testval);
	}
	// check if the test value is a perfect power
	// if so, it will be composite.
	// if not, then we will keep testing it further
	printf("Testing if %d is a perfect power\n", testval);
	double base;
	for(int b = 2; b <= log2(testval); b++){
		base = pow(testval, 1.0/b);

		if(is_whole(base)){
			printf("Input %d is a perfect power (%d^%d), therefore it is composite.\n", testval, (int)base, b);
			
			// return composite
			return 0;
		}

	}

	printf("Input %d is not a perfect power, now finding r value\n", testval);


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

	printf("Found r value %d for the input value %d\n", r, testval);

	// a is the minimum of r and n-1
	int a = r;
	if(testval-1 < r){
		a = testval-1;
	}

	for( ; a > 1; a--){
		int g = GCD(testval, a);
		if(g > 1 &&  g < testval){
			printf("Input value %d is divisible by value %d and 2 <= %d <= min(r, testval-1), so %d is composite\n", testval, a, a, testval);

			// return composite
			return 0;
		}
	}

	printf("Did not find value a where 2 <= a <= min(r, testval-1) that divides input value %d\n", testval);

	if(testval <= r){
		printf("Input value %d is less than or equal to r (%d), therefore %d is prime\n", testval, r, testval);

		// return prime
		return 1;
	}

	printf("Input value %d is not less than or equal to r value of %d\n", testval, r);	


	// time for the last part using polynomials



	// return prime
	return 1;
}