/* fft2osc
 * 
 * Compiling this on Linux :
 * gcc fft2osc.c -g -Wall -std=c99 -lsndfile -lfftw3 -lm -o fft2osc
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.h>
#include <fftw3.h>

#define PI 3.14159265358979323846


const char* infilename = "/home/johnDoe/Music/foo.wav";
const char* outfilename = "/home/johnDoe/Music/bar.wav";

// defining buffer size (for fft)
const int buffSize = 1024;

double** inSamples;
double** ampData;
double** phaseData;
double* outSamples;

void getAmplitude (int iSize, int N, fftw_complex **data) {
	
	// Creating an array of pointers to actual data
	ampData = malloc(sizeof(double*) * iSize);
	
	for (int i = 0; i < iSize; i++) {
		ampData[i] = malloc(sizeof(double) * N);
	}
	
	for (int i = 0; i < iSize; i++) {
		for (int j = 0; j < N; j++) {
			
			// calculate amplitude
			ampData[i][j] = ((sqrt( (data[i][j][0]) * (data[i][j][0]) + (data[i][j][1]) * (data[i][j][1])) ) / N);
		}
	}
}

void getPhase (int iSize, int N, fftw_complex **data) {
	phaseData = malloc(sizeof(double*) * iSize);
	
	for (int i = 0; i < iSize; i++) {
		phaseData[i] = malloc(sizeof(double) * N);
	}
	
	for (int i = 0; i < iSize; i++) {
		for (int j = 0; j < N; j++) {
			
			// calculate phase
			phaseData[i][j] = atan2(data[i][j][1], (data[i][j][0]));
		}
	}
}



int main(int argc, char **argv)
{
	int semiBuffSize = (buffSize/2)+1;
	
	// ----------
	// SNDFILE
	// ----------
	
	// libsndfile input file init
	SF_INFO inSfInfo;
    SNDFILE * infile = sf_open(infilename, SFM_READ, &inSfInfo);
    
    // partialy calculating the size of the 2d array used for sample analysis
    float flIndexSize = (float) inSfInfo.frames / (float) buffSize;
    int indexSize = (int) inSfInfo.frames / buffSize;
    
    if (flIndexSize - indexSize != 0) {
		indexSize++;
	}
	
	// declaring sample array (on the heap)
	inSamples = malloc(sizeof(double*) * indexSize);
	for (int i = 0; i < indexSize; i++) {
		inSamples[i] = malloc(sizeof(double) * buffSize);
	}
	
	// ------------
	// FFTW
	// ------------
	
	// fftw init	
	fftw_complex** fftData = malloc(sizeof(fftw_complex*) * indexSize);
	for (int i = 0; i < indexSize; i++) {
		fftData[i] = malloc(sizeof(fftw_complex) * semiBuffSize);
	}
	
	fftw_plan p;
	
	// ------------
	// LOOP
	// ------------
	
	// declaring i for the do/while loop.
	int i = 0;
	
    do {
		// infile -> inSamples[][]
		sf_count_t count = sf_read_double(infile, &inSamples[i][0], buffSize);
		
		// security mecanism for avoiding reading data "out of the array bounds"
		if ((int) count != buffSize) {
			for (int j = (int) count; j < buffSize; j++) {
				inSamples[i][j] = 0.0;
			}
		}
				
		// inSamples -> fftData
		p = fftw_plan_dft_r2c_1d(buffSize, &inSamples[i][0], &fftData[i][0], FFTW_ESTIMATE);
		fftw_execute(p);
		
		i++;
		
	} while (i < indexSize);

	
	//getAmplitude()...
	getAmplitude(indexSize, semiBuffSize, fftData);
	getPhase(indexSize, semiBuffSize, fftData);
	
	
	// libsndfile out init
	SF_INFO outSfInfo;
	outSfInfo.channels = inSfInfo.channels;
	outSfInfo.samplerate = inSfInfo.samplerate;
	outSfInfo.format = inSfInfo.format;
	
	SNDFILE* outfile = sf_open( outfilename, SFM_WRITE, &outSfInfo);
	
	// alternative buffSize value for time-stretching
	int buffSize2 = buffSize * 1;
	
	outSamples = calloc(indexSize*buffSize2, sizeof(double));
	
	double freq, currentFreq;
	
	// -------------------
	// RESYNTHESIS
	// -------------------
	
	for (int i=0; i < indexSize; i++) {
		for (int b = 0; b < buffSize2; b++) {
			for (int N = 0; N < semiBuffSize; N++) {
				
				freq = (outSfInfo.samplerate / 2.0) * ((double) N / (double) semiBuffSize);
				
				// Modify this variable to do some pitch-shifting stuff...
				currentFreq = freq * 1;
				
				if (currentFreq > outSfInfo.samplerate) {currentFreq = 0;}
				outSamples[i*buffSize2+b] += sin(b * currentFreq * ((2 * PI)/ outSfInfo.samplerate) + phaseData[i][N]) * ampData[i][N];
				
			}
		}
    }
    
    // write the entire buffer to the file
    sf_write_double (outfile, &outSamples[0], indexSize*buffSize2);
    // force write to disk
    sf_write_sync( outfile );
    // don't forget to close the file
    sf_close( outfile );
	
	
    // closing functions for avoiding memory leaks
	sf_close(infile);
	fftw_destroy_plan(p);
	fftw_free(fftData);
	free(inSamples);
	free(outSamples);
	free(ampData);
	free(phaseData);
	
	return 0;
}
