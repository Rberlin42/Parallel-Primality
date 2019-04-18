#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "aks.h"

int main(int argc, char* argv[]){
	int isprime = aks_prime(36);

	printf("%d\n", isprime);

	return 0;
}