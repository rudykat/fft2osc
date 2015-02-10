#ifndef FFT2OSC_H
#define FFT2OSC_H
#endif

#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

#define PI 3.14159265358979323846

void printHelp()
{
    printf("Usage : fft2osc [-options] sourceFile destinationFile \n \
	-s : defines FFT size (has to be a power of 2 between 8 and 65536, default is '512') \n \
	-t : defines timestretching factor ('2' makes the file 2 times longer, '0.5' 2 times shorter, default is '1') \n \
	-p : defines pitch-shifting factor ('2' makes the pitch an octave above, '0.5 an octave below, default is '1') \n \n");
}

double** getAmplitude (fftw_complex **data, int iSize, int N)
{

    // Creating an array of pointers to actual data
    double** ampData = malloc(sizeof(double*) * iSize);

    for (int i = 0; i < iSize; i++) {
        ampData[i] = malloc(sizeof(double) * N);
    }

    for (int i = 0; i < iSize; i++) {
        for (int j = 0; j < N; j++) {

            // calculate amplitude
            ampData[i][j] = ((sqrt( (data[i][j][0]) * (data[i][j][0]) + (data[i][j][1]) * (data[i][j][1])) ) / N);
        }
    }

    return ampData;
}

double** getPhase (fftw_complex **data, int iSize, int N)
{
    double** phaseData = malloc(sizeof(double*) * iSize);

    for (int i = 0; i < iSize; i++) {
        phaseData[i] = malloc(sizeof(double) * N);
    }

    for (int i = 0; i < iSize; i++) {
        for (int j = 0; j < N; j++) {

            // calculate phase
            phaseData[i][j] = atan2(data[i][j][1], (data[i][j][0]));
        }
    }

    return phaseData;
}

/*
double* triang(int N)
{
    int divisor;
    double factor[N];

    if (N % 2 == 0) {		// if N is even...
        divisor = N / 2 - 1;

        for (int i = 0; i < N / 2; i++) {
            factor[i] = i / divisor;
            factor[(N-1) - i] = i / divisor;
        }

    } else {		// if N is odd...
        divisor = (N - 1) / 2;
        factor[divisor] = 1;

        for (int i = 0; i < divisor; i++) {
            factor[i] = i / divisor;
            factor[(N-1) - i] = i / divisor;
        }
    }

    return factor;
}
*/
