#include <sndfile.h>
#include <stdlib.h>
#include <stdbool.h>

static SF_INFO in_file_info;
static SNDFILE* in_file_ptr;
static SNDFILE* out_file_ptr;
static bool file_is_readable = true;

void init_in_file(const char* path, int proto_buff_size)
{
	in_file_ptr = sf_open(path, SFM_READ, &in_file_info);
	
	if (in_file_info.channels > 2) {
		printf("Files with more than 2 channels will be uncorrectly processed");
	}
}

int get_nb_of_channels() 
{
	return in_file_info.channels;
}

int get_samplerate()
{
	return in_file_info.samplerate;
}

bool get_samples(double* buffer, int buff_size) 
{
	if (file_is_readable == false) return false;
	
	sf_count_t counter = sf_read_double(in_file_ptr, buffer, buff_size);
	
	if ((int) counter != buff_size) {
		for (int i = (int) counter; i < buff_size; i++) {
			buffer[i] = 0.0;
		}
		file_is_readable = false;
	}
	
	return file_is_readable;
}

void init_out_file(const char* path) 
{
	out_file_ptr = sf_open(path, SFM_WRITE, &in_file_info);
}

void write_out_file(double* buffer, double buff_size) 
{
	sf_write_double(out_file_ptr, buffer, buff_size);
}

void free_file_handling() 
{
	sf_close(in_file_ptr);
	sf_close(out_file_ptr);
}

void free_file_failure() 
{
	sf_close(out_file_ptr);
}
