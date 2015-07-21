/*
 * FFT2OSCs
 * (c) Rudy Kattouche, 2015
 * Released under the GNU Public Licence version 3 or later.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cli.h"
#include "file.h"
#include "processing.h"

int main(int argc, char** argv) 
{
	// --- //
	// CLI //
	// --- //
	
	Params p = get_params(argc, argv);
	
	if (p.correct_params == false) {
		print_help();
		return EXIT_FAILURE;
	}
	
	// ------------- //
	// File Handling //
	// ------------- //
	
	init_in_file(p.in_file_path, p.fft_size);
	int channels = get_nb_of_channels();
	
	if (channels > 2) {
		printf("fft2oscs can only handles mono or stereo files.\n");
		free_file_failure();
		return EXIT_FAILURE;
	}
	
	bool file_is_readable;
	const int in_size = p.fft_size; // * channels; // mono buff length = FFT size
	double in_buffer[in_size];
	
	init_out_file(p.out_file_path);
	
	int out_size = in_size * p.time_stretch_factor;
	double out_buffer[out_size];
	
	// ---------- //
	// Processing //
	// ---------- //
	
	init_fftw(p.fft_size, get_samplerate(), channels); 
	
	/* 'in_size' is calculated with those 2 variables
	 * so I don't need to pass it as an argument 
	 * for the other functions declared in the 'processing' module */
	
	do {
		file_is_readable = get_samples(in_buffer, in_size);
		if (file_is_readable == false) break;
		
		process_buff(in_buffer, p.pitch_shift_factor, out_buffer, out_size);
		write_out_file(out_buffer, out_size);
		
	} while (file_is_readable == true);
	
	// ------------------- //
	// Freeing & returning //
	// ------------------- //
	
	free_file_handling();
	free_fftw();
    return EXIT_SUCCESS;
}

// ----------------------------------------
// ----------------------------------------

/* Debug
printf("f = %d, t = %f, p = %f, i = %s\n\n", 
		p.fft_size, p.time_stretch_factor, 
		p.pitch_shift_factor, p.in_file);
*/
