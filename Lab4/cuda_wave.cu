#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cuda.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265

void check_param(void);
void init_line(void);
void update (void);
void printfinal (void);
__device__ void do_math(float*, float*, float*, int );

//---------------------------------------------------------------------------
//	Kernel function
//---------------------------------------------------------------------------
__global__ void gpu_init_line( float*, float*, int );
__global__ void gpu_update( float*, float*, float*, int, int );


int nsteps,                 	/* number of time steps */
	tpoints, 	     		/* total points along string */
	rcode;                  	/* generic return code */

float  values[MAXPOINTS+2], 	/* values at time t */
	   oldval[MAXPOINTS+2], 	/* values at time (t-dt) */
	   newval[MAXPOINTS+2]; 	/* values at time (t+dt) */

float *gValues, 
	  *gOldVal, 
	  *gNewVal;


/**********************************************************************
 *	Checks input values from parameters
 *********************************************************************/
void check_param(void)
{
	char tchar[20];

	/* check number of points, number of iterations */
	while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
		printf("Enter number of points along vibrating string [%d-%d]: "
				,MINPOINTS, MAXPOINTS);
		scanf("%s", tchar);
		tpoints = atoi(tchar);
		if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
			printf("Invalid. Please enter value between %d and %d\n", 
					MINPOINTS, MAXPOINTS);
	}
	while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
		printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
		scanf("%s", tchar);
		nsteps = atoi(tchar);
		if ((nsteps < 1) || (nsteps > MAXSTEPS))
			printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
	}

	printf("Using points = %d, steps = %d\n", tpoints, nsteps);

}

/**********************************************************************
 *     Initialize points on line
 *********************************************************************/
__global__ void gpu_init_line( float *gOld, float *gVal, int tpoints )
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;

	/* Initialize old values array */
	gOld[index] = gVal[index];
}

void init_line(void)
{
	//int i, j;
	int j;
	float x, fac, k, tmp;

	/* Calculate initial values based on sine curve */
	fac = 2.0 * PI;
	k = 0.0; 
	tmp = tpoints - 1;
	for (j = 1; j <= tpoints; j++) {
		x = k/tmp;
		values[j] = sin (fac * x);
		k = k + 1.0;
	} 

	cudaMemcpy( gValues, values, (MAXPOINTS+2)*sizeof(float),
			cudaMemcpyHostToDevice );

	dim3 dimBlock( 512 ), dimGrid( (tpoints + dimBlock.x -1) / dimBlock.x );
	gpu_init_line<<<dimGrid, dimBlock>>>( gOldVal, gValues, tpoints );
}


/**********************************************************************
 *      Calculate new values using wave equation
 *********************************************************************/
__device__ void do_math(float *gOld, float *gVal, float *gNew, int i)
{
	float dtime, c, dx, tau, sqtau;

	dtime = 0.3;
	c = 1.0;
	dx = 1.0;
	tau = (c * dtime / dx);
	sqtau = tau * tau;
	gNew[i] = (2.0 * gVal[i]) - gOld[i] + (sqtau *  (-2.0)*gVal[i]);
}

/**********************************************************************
 *     Update all values along line a specified number of times
 *********************************************************************/
__global__ void gpu_update( float *gOld, float *gVal, float *gNew, int nsteps,
		int tpoints )
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;

	/* Update values for each time step */
	for( int i=0; i < nsteps; i++ ) {
		/* global endpoints */
		if( (index == 1) || (index == tpoints) )
			gNew[index] = 0.0;
		else
			do_math( gOld, gVal, gNew, index );

		/* Update old values with new values */
		gOld[index] = gVal[index];
		gVal[index] = gNew[index];
	}
}

void update()
{

	dim3 threadsPerBlock( 512 ),
		 blocksPerGrid( (tpoints + threadsPerBlock.x -1) / threadsPerBlock.x );
	gpu_update<<<blocksPerGrid, threadsPerBlock>>>( gOldVal, gValues, gNewVal, nsteps, tpoints );

	cudaMemcpy( values, gValues, (MAXPOINTS+2)*sizeof(float),
			cudaMemcpyDeviceToHost );
}


/**********************************************************************
 *     Print final results
 *********************************************************************/
void printfinal()
{
	int i;

	for (i = 1; i <= tpoints; i++) {
		printf("%6.4f ", values[i]);
		if (i%10 == 0)
			printf("\n");
	}
}

/**********************************************************************
 *	Main program
 *********************************************************************/
int main(int argc, char *argv[])
{
	sscanf(argv[1],"%d",&tpoints);
	sscanf(argv[2],"%d",&nsteps);

	/*	Allocate space in device Global Memory	*/
	cudaMalloc( (void**)&gValues, (MAXPOINTS+2)*sizeof(float) );
	cudaMalloc( (void**)&gOldVal, (MAXPOINTS+2)*sizeof(float) );
	cudaMalloc( (void**)&gNewVal, (MAXPOINTS+2)*sizeof(float) );

	check_param();
	printf("Initializing points on the line...\n");
	init_line();
	printf("Updating all points for all time steps...\n");
	update();
	printf("Printing final results...\n");
	printfinal();
	printf("\nDone.\n\n");

	/*	Free allocated space in device Global Memory	*/
	cudaFree( gValues );
	cudaFree( gOldVal );
	cudaFree( gNewVal );

	return 0;
}
