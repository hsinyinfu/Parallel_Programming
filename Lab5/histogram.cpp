#include <iostream>
#include <string>
#include <fstream>
#include <CL/opencl.h>
using namespace std;

#define OUTPUT_FILE "0756015.out"
#define INPUT_FILE "input"

string readKernelSource( const string& );

int main( int argc, char *argv[] ){
	unsigned int input_size, pixels, pixPerThread;
	unsigned int *image, *histogram;
	size_t maxWorkItems;
	size_t maxWorkItemsEachDim[3];
	string kernelSource;

	// Open file streams
	ifstream inFile( INPUT_FILE );
	ofstream outFile( OUTPUT_FILE );

	// Read input from the input file
	inFile >> input_size;
	image = new unsigned int[input_size];
	histogram = new unsigned int[256 * 3];

	unsigned int tmp;
	int i = 0;
	while( inFile >> tmp ) {
		image[i++] = tmp;
	}


	//-----------------------------------------------------------------------
	// OpenCL code
	//-----------------------------------------------------------------------
	cl_int err;
	cl_uint num_device, num;
	cl_device_id device;
	cl_platform_id platform_id;

	// Quert platform & device
	err = clGetPlatformIDs( 1, &platform_id, &num );
	err = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device,
			&num_device );
	err = clGetDeviceInfo( device, CL_DEVICE_MAX_WORK_ITEM_SIZES,
			sizeof(maxWorkItemsEachDim), &maxWorkItemsEachDim, NULL );

	// Compute the number of work-items & pixels for each work-item
	maxWorkItems = 
		maxWorkItemsEachDim[0] 
		* maxWorkItemsEachDim[1] 
		* maxWorkItemsEachDim[2];
	pixels = input_size / 3;
	if( pixels > maxWorkItems ){
		pixPerThread = pixels / maxWorkItems;

		// work load imbalance for each work-item,
		// so we have to check the boundary in kernel function
		// when accessing the array.
		if( pixels % maxWorkItems != 0 ) pixPerThread++;
	}
	else{
		pixPerThread = 1;
	}

	// Create context
	cl_context context = clCreateContext( NULL, 1, &device, NULL, NULL, &err );

	// Create command queue
	cl_command_queue cmdQ = clCreateCommandQueue( context, device, 0, &err );

	// Create Buffers
	cl_mem imgBuf = clCreateBuffer( context, CL_MEM_READ_ONLY, 
			sizeof(unsigned int) * input_size, NULL, &err );
	cl_mem histBuf = clCreateBuffer( context, CL_MEM_WRITE_ONLY,
			sizeof(unsigned int) * 256 * 3, NULL, &err );

	// Transfer data from host to the device memory
	clEnqueueWriteBuffer( cmdQ, imgBuf, CL_TRUE, 0,
			sizeof(unsigned int) * input_size, image, 0, NULL, NULL );

	// Create program
	kernelSource = readKernelSource( "histogram_kernel.cl" );
	const char *source = kernelSource.c_str();
	size_t sourceSize = kernelSource.size();

	cl_program program = clCreateProgramWithSource( context, 1,
			&source, &sourceSize, &err );
	if( err != CL_SUCCESS )
		cerr << "[ Create program With Source ] Failed." <<endl;

	// Compile program
	err = clBuildProgram( program, 1, &device, NULL, NULL, NULL );
	cl_int ret = clGetProgramBuildInfo( program, device, 
			CL_PROGRAM_BUILD_STATUS, 0, NULL, NULL);
	if( ret != CL_BUILD_SUCCESS )
		cerr << "[ Build Program ] Failed." << endl;

	// Create kernel
	cl_kernel kernel = clCreateKernel( program, "histogram", &err );
	if( err != CL_SUCCESS )
		cerr << "[ Create Kernel ] Failed." << endl;
	if( err == CL_INVALID_PROGRAM_EXECUTABLE )
		cerr << "[ Create Kernel ] Invalid program executable." << endl;


	// Set kernel arguments
	clSetKernelArg( kernel, 0, sizeof(cl_mem), &imgBuf );
	clSetKernelArg( kernel, 1, sizeof(cl_mem), &histBuf );
	clSetKernelArg( kernel, 2, sizeof(unsigned int), &pixels );
	clSetKernelArg( kernel, 3, sizeof(unsigned int), &pixPerThread );

	// Execute kernel
	err = clEnqueueNDRangeKernel( cmdQ, kernel, 1, 0, &maxWorkItems, 
			0, 0, NULL, NULL );
	if( err != CL_SUCCESS )
		cerr << "[ Enqueue ND Range Kernel ] Failed." << endl;
	if( err == CL_INVALID_KERNEL )
		cerr << "[ Enqueue ND Range Kernel ] Invalid kernel." << endl;

	// Transfer the result from device to host
	clEnqueueReadBuffer( cmdQ, histBuf, CL_TRUE, 0, 
			sizeof(unsigned int) * 256 * 3, histogram, 0, NULL, NULL );

	// Write the result to the output stream
	for( int i=0; i < 768; i++ ){
		outFile << histogram[i] << " ";
		if( i == 255 || i == 511 )
			outFile << endl;
	}

	// Close the file streams and free the memory space
	inFile.close();
	outFile.close();
	delete image;
	delete histogram;
}

string readKernelSource( const string& path ){
	string source = "", tmp="";
	ifstream in;
	in.open( path.c_str() );
	while( getline( in, tmp ) ){
		source += tmp;
		source += "\n";
	}
	in.close();
	return source;
}
