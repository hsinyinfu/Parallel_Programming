__kernel void histogram( 
		__global unsigned int *img, 
		__global unsigned int *hist, 
		unsigned int nPixel,
		unsigned int pixPerThread)
{
	unsigned int i, j, index;
	const int idx = get_global_id(0);

	unsigned int start = idx * pixPerThread,
				 end = (idx+1) * pixPerThread - 1;
	
	for( i = start; i <= end; i++ ){
		if( i < nPixel ){
			for( j = 0; j < 3; j++ ){
				index = img[ i*3+j ]; 
				atomic_inc( hist+(index + j*256) );
			}
		}	   
	}
}

