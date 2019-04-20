#ifndef AKS_H
#define AKS_H



/**
 * calculates the factorial of n, n must be >= 0
 *
 */
long long factorial(int n);

/**
 * calculates the combination of nCr
 *
 */
int nCr(int n, int r);

/**
 * raise (X + c) to a given power, stores it in the array poly
 * inputs: constant c, power to raise to, and polynomial array
 *		requires: the size of poly must be (power+1)
 */
void polyPower(int c, int power, long long* poly);


/**
 * calculates the Euler phi of a given number
 * this is used during the AKS primality test
 *
 */
int phi(unsigned int n);


/**
 * calculate the multiplicative order ord_n(a)
 * output: returns the number k where a^k === 1 (mod n), or returns -1 if such a k does not exist
 * 
 */
int ord(int a, int n);

/**
 * calculate the GCD of a and b
 *
 */
int GCD(int a, int b);


/**
 * tells us whether the input is a whole number; i.e. has no fractional part
 * input: the number we are testing, type double
 * output: 1 if it is whole, 0 if there is any fractional component
 *
 */
int is_whole(double x);


/**
 * aks_prime: takes in a number and tells us whether it is prime or composite
 * input: positive long integer > 1, this is the value we are checking if it's prime
 * output: 1 if the input is prime, 0 if the input is composite
 *
 */
int aks_prime(int testval);

#endif