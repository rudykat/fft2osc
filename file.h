#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

int init_in_file(const char* path);

//int get_nb_of_channels();
int get_samplerate();

bool channel_to_mem();
bool get_samples(double* buffer, int fft_size);

void init_out_file(const char* path);
void write_out_file(double* buffer, long size);

void free_file_handling();
// void free_file_failure();

#endif
