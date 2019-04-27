# Parallel-Primality
Parallel Programming Final Project

trialDivision.c: 
>> Complile on BG/Q using mpixlc and -lm command.
>> To edit input size, go into trialDivision.c and on line 49, change the varibale "ull primes" to the input size of your liking
>> For strong scaling, run using ull primes = 2^30. Run with number of ranks between 64 and 8192 (power of 2 only).
>> For weak scaling, Adjusts the test size based on the number of ranks. 64 ranks has an input of 2^15, and 8192 ranks has an input size of 2^22. Outputs the number of primes for the input size. 
miller_rabin.c



aks-strong.c: 
	Compile on BG/Q using mpixlc and the -lm command for strong scaling tests. Uses an input size of 2^22,
	and outputs the number of primes between 2 and 2^22. Run with number of ranks between 64 and 8192 (power of 2 only).

aks-weak.c:
	Compile on BG/Q with mpixlc using the -lm command to run weak scaling. Adjusts the test size based on the number of 
	ranks. 64 ranks has an input of 2^15, and 8192 ranks has an input size of 2^22. Outputs the number of primes for the input size. Run with an amount of ranks which are powers of 2 between 64 and 8192.

aks.h:
	function header used by the aks-strong and aks-weak functions
