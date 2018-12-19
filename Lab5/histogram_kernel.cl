__kernel void histogram( 
		__global unsigned int *img, 
		__global unsigned int *hist, 
		unsigned int size )
{
	const int idx = get_global_id(0);
	int i;

	// Count one of the RED
	if( idx < 256 ){
		for( i = 0; i < size; i++ ){
			if( i%3==0 && img[i]==idx ){
				hist[idx]++;
			}
		}
	}

	// Count one of the GREEN
	else if( idx >= 256 && idx <512 ){
		for( i = 0; i < size; i++ ){
			if( i%3==1 && img[i]==(idx-256) ){
				hist[idx]++;
			}
		}
	}

	// Count one of the BLUE
	else if( idx >= 512 && idx < 768 ){
		for( i = 0; i < size; i++ ){
			if( i%3==2 && img[i]==(idx-512) ){
				hist[idx]++;
			}
		}
	}
}
