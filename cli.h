#ifndef CLI_H_INCLUDED
#define CLI_H_INCLUDED

#include <stdbool.h>

typedef struct 
{
	bool correct_params;
	const char* in_file_path;
	const char* out_file_path;
	int fft_size;
	double pitch_shift_factor;
	double time_stretch_factor;
} Params;
	
void print_help();
Params get_params(int argc, char** argv);

#endif
