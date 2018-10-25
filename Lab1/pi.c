#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


long long int dartInCircle = 0;
long long int baseRound = 0;
int rest = 0;
pthread_mutex_t mux;


void *tossDart( void *threadNum );


int main( int argc, char *argv[] ){

	if( argc != 3 ){
		printf("The number of parameters doesn't match.\n");
		return -1;
	}

	int cpuNum = atoi( argv[1] );
	long long int tossNum = atol( argv[2] );

	baseRound = tossNum / cpuNum;	// The number of rounds every thread must do

	rest = tossNum % cpuNum;		/* The remaining rounds n which occurs when
									   (tossNum/cpuNum) != 0 is done 
									   by first n threads */

	pthread_mutex_init( &mux, NULL );

	pthread_t *threadHandles = malloc( cpuNum * sizeof(pthread_t) );


	for( long thread=0; thread < cpuNum; thread++ ){
		pthread_create( &threadHandles[thread], NULL, tossDart, 
				(void *)thread );
	}

	for( long thread=0; thread < cpuNum; thread++ ){
		pthread_join( threadHandles[thread], NULL );
	}

	double pi = ( 4.0 * dartInCircle ) / tossNum;
	printf( "%lf\n", pi );
	
	free( threadHandles );
	pthread_mutex_destroy( &mux );

	return 0;

}

void *tossDart( void *threadNum ){

	int x, y;
	long thread = (long) threadNum;
	long long int localDartInCircle = 0;
	long long int round = ( (thread < rest)? baseRound+1 : baseRound );
	double radius = pow( RAND_MAX, 2 );
	double distance = 0;

	unsigned int seed = time( NULL );

	for( long long int i=0; i < round; i++ ){

		x = rand_r( &seed );
		y = rand_r( &seed );

		distance = pow( x, 2 ) + pow( y, 2 );
		if( distance <= radius )
			localDartInCircle++;

	}

	/* critical section */
	pthread_mutex_lock( &mux );
	dartInCircle += localDartInCircle;
	pthread_mutex_unlock( &mux );

	return NULL;

}
