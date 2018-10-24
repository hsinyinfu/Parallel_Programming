#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char *argv[] ){

	if( argc != 2 ){
		printf("The number of parameters doesn't match.\n");
		return -1;
	}

	long long int dartInCircle = 0;
	long long int tossNum = atol( argv[1] );

	srand( time(NULL) );

	double x, y;

	for( long long int i=0; i < tossNum; i++ ){
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;

		if( sqrt( pow( x, 2 ) + pow( y, 2 ) ) <= 1.0 ){
			dartInCircle++;
		}

	}

	double pi = ( 4.0 * dartInCircle ) / tossNum;
	printf( "pi: %lf\n", pi );

	return 0;

}
