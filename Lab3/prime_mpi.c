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
		locRank,
		size,
		partialPC = 0,
		partialFO = 0,
		locPC = 0,
		tag = 0;
	long long int n=0, limit, avgWork, remainWork, start, end;
	MPI_Status status;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &locRank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	if( locRank == 0 ){
		sscanf(argv[1],"%llu",&limit);	
		printf("Starting. Numbers to be scanned= %lld\n",limit);
	}

	pc=4;     /* Assume (2,3,5,7) are counted here */
	avgWork = (limit - 11 + 1) / (size - 1);
	remainWork = (limit - 11 + 1) % (size - 1);

	if( locRank == 0 ) {
		for( int id=1; id < size; id++ ) {
			if( id <= remainWork ) {
				start = 11 + (id-1)*avgWork + id - 1;
				end = 11 + id*avgWork + id - 1;
			} else {
				start = 11 + (id-1)*avgWork + remainWork;
				end = 11 + id*avgWork + remainWork - 1;
			}
			MPI_Send( &start, 1, MPI_LONG_LONG, id, tag, MPI_COMM_WORLD );
			MPI_Send( &end, 1, MPI_LONG_LONG, id, tag, MPI_COMM_WORLD );
			MPI_Send( &avgWork, 1, MPI_LONG_LONG, id, tag, MPI_COMM_WORLD );
			MPI_Send( &remainWork, 1, MPI_LONG_LONG, id, tag, MPI_COMM_WORLD );
		}

		for( int i=1; i < size; i++ ) {
			MPI_Recv( &partialFO, 1, MPI_INT, i, tag, MPI_COMM_WORLD,
					&status );
			MPI_Recv( &partialPC, 1, MPI_INT, i, tag, MPI_COMM_WORLD,
					&status );
			//printf("received partial fo: %d, partial pc: %d\n", partialFO,
			//		partialPC);
			pc += partialPC;
			if( partialFO > foundone )
				foundone = partialFO;
		}
	}
	else {

		MPI_Recv( &start, 1, MPI_LONG_LONG, 0, tag, MPI_COMM_WORLD, &status );
		MPI_Recv( &end, 1, MPI_LONG_LONG, 0, tag, MPI_COMM_WORLD, &status );
		MPI_Recv( &avgWork, 1, MPI_LONG_LONG, 0, tag, MPI_COMM_WORLD, &status );
		MPI_Recv( &remainWork, 1, MPI_LONG_LONG, 0, tag, MPI_COMM_WORLD, &status );

		//printf("rank:%d, start: %lld, end: %lld, avgWork:%lld, remain:%lld, pc:%d, foundone:%d\n", 
		//		locRank, start, end, avgWork, remainWork, partialPC, partialFO );
		for( long long int i = start; i <= end; i++ ) {
			if( i % 2 == 0 )
				continue;
			if( isprime(i) ){
				partialPC++;
				partialFO = i;
			}
		}
		//printf("rank %d's partial foundone: %d; parital pc: %d\n",
		//		locRank, partialFO, partialPC);

		MPI_Send( &partialFO, 1, MPI_INT, 0, tag, MPI_COMM_WORLD );
		MPI_Send( &partialPC, 1, MPI_INT, 0, tag, MPI_COMM_WORLD );

	}

	if( locRank == 0 )
		printf("Done. Largest prime is %d Total primes %d\n",foundone,pc);
	MPI_Finalize();

	return 0;
} 
