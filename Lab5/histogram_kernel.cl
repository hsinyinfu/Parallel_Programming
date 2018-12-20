__kernel void histogram( 
		__global unsigned int *img, 
		__global unsigned int *hist, 
		unsigned int size )
{
	const int idx = get_global_id(0);
	int start, bar;

	// Count one bar of RED
	if( idx < 256 ){
		start = 0;
		bar = idx;
	}
	// Count one bar of GREEN
	else if( idx >= 256 && idx < 512 ){
		start = 1;
		bar = idx - 256;
	}
	// Count one bar of BLUE
	else{
		start = 2;
		bar = idx - 512;
	}

	for( int i = start; i < size; i+=3 ){
		if( img[i] == bar )
			hist[idx]++;
	}
}
