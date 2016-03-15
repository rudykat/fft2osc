#include <sndfile.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static SF_INFO in_info;
static SNDFILE* in_ptr;
static SNDFILE* out_ptr;
static sf_count_t remaining_frames;
static double* ch_samples; // = channel samples

int init_in_file(const char* path)
{
        in_info.format = 0; // see libsndfile dpc
        in_ptr = sf_open(path, SFM_READ, &in_info);
	if (in_ptr == NULL)
	        return -1;
	// else
	ch_samples = malloc(sizeof(double) * in_info.frames);
	return in_info.channels;
}

int get_samplerate()
{
	return in_info.samplerate;
}

bool channel_to_mem()
{
	remaining_frames = sf_read_double(in_ptr, ch_samples, in_info.frames);
	if (remaining_frames == 0) return false;
	else return true;
}

bool get_samples(double* buffer, int fft_size)
{
        sf_count_t index = in_info.frames - remaining_frames;

	// Regular case
	if (remaining_frames > fft_size) {
	        memcpy(buffer, (double*) ch_samples + index, fft_size * sizeof(double));
		remaining_frames -= fft_size;
		index += fft_size;
	}

	// Defense against out of bounds memory-reading
	else if (remaining_frames == 0) {
	        return false; // Stop reading the memory
	}
	// Add some zeros at the end of buffer, if in_info.frames % fft_size != 0
	else if (remaining_frames < fft_size) {
		memcpy(buffer, (double*) ch_samples + index, remaining_frames * sizeof(double));
		for (int i = fft_size-1; i >= remaining_frames; i--) {
			buffer[i] = 0.0;
		}
		// remaining_frames -= fft_size;
		return false;
	}
	
	// else
	return true; // main() can continue to read the mem
}

void init_out_file(const char* path)
{
        SF_INFO info_copy = in_info; // needed, otherwise channel_to_mem() will fail...
        out_ptr = sf_open(path, SFM_WRITE, &info_copy);
}

void write_out_file(double* buffer, long size)
{
        sf_write_double(out_ptr, buffer, size);
}

void free_file_handling()
{
	free(ch_samples);
	sf_close(in_ptr);
	sf_close(out_ptr);
}
/*
void free_file_failure()
{
	sf_close(in_ptr);
}
*/
