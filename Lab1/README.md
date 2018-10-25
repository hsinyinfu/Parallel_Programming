# Pthread Programming

Using Pthread to accelerate the computation of pi in Monte Carlo method.

## Compile
To compile the `pi.c`, which is a parallel version of computing pi:
```
gcc -pthread -std=gnu99 -O2 -s pi.c -o pi
```

To compile the `serial_pi.c` as a baseline of speed:
```
gcc -std=gnu99 -O2 -s serial_pi.c -o baseline
```

## Run
The `pi.c` takes two command-line arguments, which indicate the number of CPU cores and the number of rounds. If I want to use `2` cores and try `100000000` rounds:
```
./pi 2 100000000
```

The `serial_pi.c` takes only one command-line arguments, which indicate how many rounds to try. If I want to try `100000000` rounds:
```
./baseline 100000000
```

## Measuring the time
If you want to measure how long does a program run, use `time` in Unix-like system. For example:
```
time ./pi 2 100000000
```

To make the the program run on certain cores, use `taskset` command. For example, if I want the two threads run on the CPU cores 1 and 2:
```
taskset -c 1,2 ./pi 2 100000000
```

To monitor the system environment and hardware usage, use `top` or `htop`(better).
