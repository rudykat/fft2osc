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

	int channels = init_in_file(p.in_file_path);
	if (channels == -1)
	    return EXIT_FAILURE;
	// else
	bool chan_is_readable;
	double in_buffer[p.fft_size];

	init_out_file(p.out_file_path);
	long outsize = p.fft_size * p.time_stretch_factor;
	double out_buffer[outsize];

	// ---------- //
	// Processing //
	// ---------- //

	init_fftw(p.fft_size, get_samplerate(), channels);

	while (channel_to_mem()) {
		do {
		    chan_is_readable = get_samples(in_buffer, p.fft_size);
			if (!chan_is_readable) break;

			process_buff(in_buffer, p.pitch_shift_factor, out_buffer, outsize);
			write_out_file(out_buffer, outsize);

		} while (chan_is_readable);
	}

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
