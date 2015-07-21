#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"

void print_help() 
{
    printf("Usage : fft2osc [-options] sourceFile [destinationFile] \n \
    -h : Print this message \n \
	-s : Defines the FFT size (has to be a power of 2 between 16 and 65536, default is '512') \n \
	-t : Defines the time-stretching factor ('2' makes the file 2 times longer, '0.5' 2 times shorter, default is '1') \n \
	-p : Defines the pitch-shifting factor ('2' rises the pitch an octave above, '0.5 an octave below, default is '1') \n \
	If no path is specified for the destination file, the program will write to './out.wav'. \n\n");
}

Params get_params(int argc, char** argv)
{
	Params params;
    int opt;
    int s;
    
    // Default parameter values
    params.correct_params = true;
    params.fft_size = 512;
	params.pitch_shift_factor = 1;
	params.time_stretch_factor = 1;
    
    while ((opt = getopt(argc, argv, "s:t:p:h")) != -1) {
        switch (opt) {
        case 's' :
			s = atoi(optarg);
			if ((s & (s-1)) != 0 || s < 16 || s > 65536) {
				params.correct_params = false;
			}
            params.fft_size = atoi(optarg);
            break;
            
        case 't' :
            if (atof(optarg) <= 0) {
                params.correct_params = false;
            }
            params.time_stretch_factor = atof(optarg);
            break;
            
        case 'p' :
            if (atof(optarg) <= 0) {
                params.correct_params = false;
            }
            params.pitch_shift_factor = atof(optarg);
            break;
            
        case 'h' :
			params.correct_params = false;
        }
    }

    if (argv[optind] == NULL) {
		params.correct_params = false;
	} 
	else {
		params.in_file_path = argv[optind];
	}
    
    if (argv[optind + 1] == NULL) {
        params.out_file_path = "./out.wav";
    } 
    else {
		params.out_file_path = argv[optind + 1];
	}
	

}
