P=fft2oscs
OBJECTS=main.o
CFLAGS= -std=gnu99 -g -Wall -Wpedantic -O2
LDFLAGS= -lm -lsndfile -lfftw3
CC=clang

all: $(P)

$(P): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ *.c

clean: 
	rm -rf *.o
