P=fft2osc
OBJECTS=fft2osc.o
CFLAGS= -std=gnu11 -g -Wall -O3
LDFLAGS= -lm -lsndfile -lfftw3
CC=gcc

all: $(P)

$(P): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ *.c

clean: 
	rm -rf *.o
