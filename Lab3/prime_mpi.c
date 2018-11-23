#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int isprime(int n) {
	int i,squareroot;
	if (n>10) {
		squareroot = (int) sqrt(n);
		for (i=3; i<=squareroot; i=i+2)
			if ((n%i)==0)
				return 0;
		return 1;
	}
	else
		return 0;
}

int main(int argc, char *argv[])
{
	int pc,       /* prime counter */
		foundone, /* most recent prime found */
		rank,
		procNum,
		partialPC = 0,
		partialFO = 0;
	long long int n=0, limit, avgWork, remainWork, start, end;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &procNum );

	if( rank == 0 ){
		sscanf(argv[1],"%llu",&limit);	
		printf("Starting. Numbers to be scanned= %lld\n",limit);
	}

	MPI_Bcast( &limit, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD );

	for( n = 11+2*rank; n <= limit; n += 2*procNum ) {
		/********************************************************************
		 * n must be an odd number starting from 11.
		 ********************************************************************/
		if( isprime( n ) ) {
			partialPC++;
			partialFO = n;
		}
	}

	MPI_Reduce( &partialPC, &pc, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
	if( rank == 0 )
		pc += 4;	/* Assume (2,3,5,7) are counted here */
	MPI_Reduce( &partialFO, &foundone, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD );


	if( rank == 0 )
		printf("Done. Largest prime is %d Total primes %d\n",foundone,pc);

	MPI_Finalize();

	return 0;
} 
