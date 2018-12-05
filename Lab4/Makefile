all: cuda_wave serial_wave

CUDACC = nvcc
CC = gcc

cuda_wave: cuda_wave.o
	$(CUDACC) -o cuda_wave cuda_wave.o -lm
cuda_wave.o: cuda_wave.cu
	$(CUDACC) -c cuda_wave.cu
serial_wave: serial_wave.o
	$(CC) -o serial_wave serial_wave.o -lm
serial_wave.o: serial_wave.c
	$(CC) -c serial_wave.c

clean:
	rm -rf *.o serial_wave cuda_wave
