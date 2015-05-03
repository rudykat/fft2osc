P=fft2osc
OBJECTS=fft2osc.o
CFLAGS= -std=gnu11 -g -Wall -O0
LDFLAGS= -lm -lsndfile -lfftw3
CC=clang

all: $(P)

$(P): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ *.c

clean: 
	rm -rf *.o
