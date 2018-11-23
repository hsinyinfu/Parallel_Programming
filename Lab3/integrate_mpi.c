#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535

int main(int argc, char **argv) 
{
  long long i, num_intervals, start;
  double rect_width, area, sum, x_middle, local_sum; 
  int rank, procNum;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &procNum );

  if( rank == 0 )
  	sscanf(argv[1],"%llu",&num_intervals);

  MPI_Bcast( &num_intervals, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD );

  rect_width = PI / num_intervals;

  sum = 0;
  local_sum = 0;
  start = rank + 1;
  for(i = start; i < num_intervals + 1; i+=procNum) {

    /* find the middle of the interval on the X-axis. */ 

    x_middle = (i - 0.5) * rect_width;
    area = sin(x_middle) * rect_width; 
    local_sum = local_sum + area;
  } 

  MPI_Reduce( &local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );

  if( rank == 0 )
	  printf("The total area is: %f\n", (float)sum);

  MPI_Finalize();

  return 0;
}   
