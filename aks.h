#ifndef AKS_H
#define AKS_H


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