#include <fftw3.h>
#include <math.h>

#define PI 3.14159265358979323846

static int N;	// = FFT size | Nb of partials = N/2+1
static int channels;
static int samplerate;

static fftw_complex *fft_data;
static fftw_plan p;

void init_fftw(int fft_size, int sr, int ch)
{
	N = fft_size;
	fft_data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (N/2+1));
	channels = ch;
	samplerate = sr;
}

static void get_amplitude(double* amp_data)
{
	for (int i = 0; i < N/2+1; i++) {
		amp_data[i] = sqrt(pow(fft_data[i][0], 2) + pow(fft_data[i][1], 2)) / (N/2+1);
	}
}

static void get_phase(double* phase_data)
{
	for (int i = 0; i < N/2+1; i++) {
		phase_data[i] = atan2(fft_data[i][1], fft_data[i][0]);
	}
}

void process_buff(double* in, int p_shift, double* out, int out_size)
{
	// buff -> FFTw plan -> getAmp & getPhase
	p = fftw_plan_dft_r2c_1d(N, in, fft_data, FFTW_ESTIMATE);
	fftw_execute(p);
    
    double amp[N/2+1], phase[N/2+1];
    get_amplitude(amp);
    get_phase(phase);
    
    // Amp & phase -> sin(...) => oscillations
    
    double freq;
    
    for (int i = 0; i < out_size; i++) {
		out[i] = 0;
		
		for (int j = 0; j < N/2+1; j++) {
			freq = (samplerate / 2) * (j / (N/2+1));
			freq *= p_shift;
			
			if (freq > samplerate / 2) break;
			out[i] += sin(i * freq * ((2 * PI) / samplerate) + phase[j]) * amp[j];
		}
	}
}

/*
void process_buff(double* in_buff, int p_shift, double* out_buff, int out_size)
{
	if (channels == 1) { // Mono
		process_mono_buff(in_buff, p_shift, out_buff, out_size);
	} 
	else { // Stereo 
		double l_in[N], r_in[N]; 
		double l_out[out_size/2], r_out[out_size/2];
		
		// Deinterleave  samples
		for (int i = 0; i < N-1; i++) {
			l_in[i] = in_buff[i*2];
			r_in[i] = in_buff[i*2 + 1];
		}
		
		// Process both channels
		process_mono_buff(l_in, p_shift, l_out, out_size/2);
		process_mono_buff(r_in, p_shift, r_out, out_size/2);
		
		
		// Re-interleave samples
		for (int i = 0; i < (out_size/2)-1; i++) {
			out_buff[i*2] = l_out[i];
			out_buff[i*2+1] = r_out[i];
		}
	}
}
*/

void free_fftw() 
{
	fftw_destroy_plan(p);
	fftw_free(fft_data);
}
