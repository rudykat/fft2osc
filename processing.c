#include <fftw3.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

static int N;	// = FFT size | Nb of partials = N/2+1
static int channels;
static double samplerate;

static fftw_complex* fft_data;
static fftw_plan p;

static void clear_buff (double* buff, long size);
static void hanning (double* buff, int size);

void init_fftw (int fft_size, int sr, int ch)
{
	N = fft_size;
	fft_data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (N/2+1));
	channels = ch;
	samplerate = (double) sr;
}

static void get_amplitude (double* amp_data)
{
	for (int i = 0; i < N/2+1; i++) {
		amp_data[i] = sqrt(pow(fft_data[i][0], 2) + pow(fft_data[i][1], 2)) / (N/2+1);
	}
}

static void get_phase (double* phase_data)
{
	for (int i = 0; i < N/2+1; i++) {
		phase_data[i] = atan2(fft_data[i][1], fft_data[i][0]);
	}
}

void process_buff (double* in, int p_shift, double* out, long out_size)
{
    // buff -> hann() -> FFTw plan -> getAmp & getPhase
    double freq;
    double amp[N/2+1], phase[N/2+1];

    p = fftw_plan_dft_r2c_1d(N, in, fft_data, FFTW_ESTIMATE);
    fftw_execute(p);

    get_amplitude(amp);
    get_phase(phase);
    clear_buff(out, out_size);

    // Amp & phase -> sin(...) => oscillations

    for (int i = 0; i < N/2+1; i++) {
	freq = i * (samplerate / 2.0) / (N/2+1);
	freq *= p_shift;
	
	// printf("%f\n", freq);
	if (freq > samplerate / 2) break;
	// else

	for (int j = 0; j < out_size; j++) {
	    out[j] += sin(j * freq * (2.f * PI / samplerate) + phase[i]) * amp[i];
	}
    }

    // May fix clicks
    /*
    out[0] *= 0.0;
    out[1] *= 0.25;
    out[2] *= 0.5;
    out[3] *= 0.75;

    out[out_size-4] *= 0.75;
    out[out_size-3] *= 0.5;
    out[out_size-2] *= 0.25;
    out[out_size-1] *= 0.0;
    */
}

static void clear_buff (double* buff, long size) {
    // clear an array located on the stack
    for (int i = 0; i < size; i++) {
	buff[i] = 0;
    }
}

static void hanning (double* buff, int size) {
    for (int i = 0; i < size; i++) {
	buff[i] *= 0.5 * (1 - cos((2 * PI * i) / (size - 1)));
    }
}

void free_fftw()
{
	fftw_destroy_plan(p);
	fftw_free(fft_data);
}
