#ifndef PROCESSING_H_INCLUDED
#define PROCESSING_H_INCLUDED

void init_fftw(int fft_size, int sr, int ch);

void process_buff(double* in, int p_shift, double* out, int out_size) ;

void free_fftw();

#endif
