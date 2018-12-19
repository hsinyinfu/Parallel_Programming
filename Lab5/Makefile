CC = g++

all: histogram histogram_serial

histogram: histogram.o
	$(CC) -o $@ $^ -lOpenCL
histogram.o: histogram.cpp histogram_kernel.cl
	$(CC) -c $<

histogram_serial: image-histogram.o
	$(CC) -o $@ $^
%.o: %.cpp
	$(CC) -c $^

clean:
	rm -rf *.o histogram_serial histogram
