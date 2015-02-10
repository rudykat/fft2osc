/* fft2osc
 *
 * Compiling this on Linux :
 * gcc fft2osc.c -Wall -Wextra -Wpedantic -g -std=gnu99 -lsndfile -lfftw3 -lm -o fft2osc
 *
 */

// TODO : command line interface + cleaning up the code (putting less stuff in main()) + triang() / overlap & add + efficient main()

#include <stdio.h>
#include <sndfile.h>
#include <unistd.h>
#include "fft2osc.h"

// defining buffer size (for fft)
int fftSize = 512;

// pitch-shifting and time-stretching factors
double pitchShiftFactor = 1;
double timeStretchFactor = 1;

double* inSamples;
double** ampData;
double** phaseData;
double* outSamples;

int main(int argc, char **argv)
{

    // ------------------
    // COMMAND LINE ARGUMENTS PARSING
    // ------------------

    char* infilename;
    char* outfilename;
    int opt;

    while ((opt = getopt(argc, argv, "s:t:p:")) != -1) {
        switch (opt) {
        case 's' :
            fftSize = atoi(optarg);
            break;
        case 't' :
            if (atof(optarg) <= 0) {
                return EXIT_FAILURE;
            }
            timeStretchFactor = atof(optarg);
            break;
        case 'p' :
            if (atof(optarg) <= 0) {
                return EXIT_FAILURE;
            }
            pitchShiftFactor = atof(optarg);
            break;
        default :
            printHelp();
            return EXIT_FAILURE;
        }
    }

    if (argv[optind] == NULL || argv[optind + 1] == NULL) {
        printHelp();
        return EXIT_FAILURE;
    }

    infilename = argv[optind];
    outfilename = argv[optind + 1];

    // ---------
    // ERROR CHECKING
    // ---------

    if ((fftSize & (fftSize-1)) != 0 && fftSize < 8 && fftSize > 65536) {
        printHelp();
        return EXIT_FAILURE;
    }

    // ---------
    // VAR INIT
    // ---------

    // FFT on N real numbers gives an output of N/2+1 complex numbers
    int fftOutputSize = (fftSize/2)+1;

    // ----------
    // SNDFILE
    // ----------

    // libsndfile input file init
    SF_INFO inSfInfo;
    SNDFILE *infile = sf_open(infilename, SFM_READ, &inSfInfo);

    // partialy calculate the size of the 2d array used for sample analysis
    float flIndexSize = (float) inSfInfo.frames / (float) fftSize;
    int indexSize = (int) inSfInfo.frames / fftSize;

    if (flIndexSize - indexSize != 0) {
        indexSize++;
    }

    // declaring sample array (on the heap)
    inSamples = malloc(sizeof(double) * fftSize);

    // ------------
    // FFTW
    // ------------

    // fftw init
    fftw_complex** fftData = fftw_malloc(sizeof(fftw_complex*) * indexSize);
    for (int i = 0; i < indexSize; i++) {
        fftData[i] = fftw_malloc(sizeof(fftw_complex) * fftOutputSize);
    }

    // declaring FFTW's plan
    fftw_plan p;

    // ------------
    // LOOP
    // ------------

    // declaring i for the do/while loop.
    int i = 0;

    do {
        // input file -> inSamples[][]
        sf_count_t count = sf_read_double(infile, &inSamples[0], fftSize);

        // security mecanism for avoiding reading data "out of the array bounds"
        if ((int) count != fftSize) {
            for (int j = (int) count; j < fftSize; j++) {
                inSamples[j] = 0.0;
            }
        }

        // inSamples -> fftData
        p = fftw_plan_dft_r2c_1d(fftSize, &inSamples[0], &fftData[i][0], FFTW_ESTIMATE);
        fftw_execute(p);

        i++;

    } while (i < indexSize);

    free(inSamples);

    //getAmplitude() + getPhase()
    double** ampData = getAmplitude(fftData, indexSize, fftOutputSize);
    double** phaseData = getPhase(fftData, indexSize, fftOutputSize);


    // libsndfile out init
    SF_INFO outSfInfo;
    outSfInfo.channels = inSfInfo.channels;
    outSfInfo.samplerate = inSfInfo.samplerate;
    outSfInfo.format = inSfInfo.format;

    SNDFILE* outfile = sf_open(outfilename, SFM_WRITE, &outSfInfo);

    // alternative buffSize value for time-stretching
    int buffSize2 = (int) (fftSize * timeStretchFactor);

    outSamples = calloc(indexSize * buffSize2, sizeof(double));

    double freq;

    // -------------------
    // RESYNTHESIS
    // -------------------

    for (int i=0; i < indexSize; i++) {
        for (int b = 0; b < buffSize2; b++) {
            for (int N = 0; N < fftOutputSize; N++) {

                freq = (outSfInfo.samplerate / 2.0) * ((double) N / (double) fftOutputSize);

                // Modify this variable to do some pitch-shifting stuff...
                freq *= pitchShiftFactor;

                if (freq > outSfInfo.samplerate / 2) {
                    break;
                }
                outSamples[i*buffSize2+b] += sin(b * freq * ((2 * PI) / outSfInfo.samplerate) + phaseData[i][N]) * ampData[i][N];

            }
        }
    }


    // write the entire buffer to the file
    sf_write_double (outfile, &outSamples[0], indexSize*buffSize2);
    // force write to disk
    sf_write_sync( outfile );
    // don't forget to close the file
    sf_close( outfile );

    // "closing" functions for avoiding memory leaks
    sf_close(infile);
    fftw_destroy_plan(p);
    fftw_free(fftData);
    free(outSamples);
    free(ampData);
    free(phaseData);

    return EXIT_SUCCESS;
}
