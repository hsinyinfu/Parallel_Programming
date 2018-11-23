mpi: prime_mpi integrate_mpi prime_serial integrate_serial

MPICC = mpicc
CC = gcc

prime_mpi: prime_mpi.o
	$(MPICC) -o prime_mpi prime_mpi.o -lm
integrate_mpi: integrate_mpi.o
	$(MPICC) -o integrate_mpi integrate_mpi.o -lm
prime_serial: prime_serial.o
	$(CC) -o prime_serial prime_serial.o -lm
integrate_serial: integrate_serial.o
	$(CC) -o integrate_serial integrate_serial.o -lm
prime_mpi.o: prime_mpi.c
	$(MPICC) -c prime_mpi.c
integrate_mpi.o: integrate_mpi.c
	$(MPICC) -c integrate_mpi.c
prime_serial.o: prime_serial.c
	$(CC) -c prime_serial.c
integrate_serial.o: integrate_serial.c
	$(CC) -c integrate_serial.c

clean: 
	rm -f *.o prime_mpi integrate_mpi prime_serial integrate_serial
