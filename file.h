#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void init_in_file(const char* path, int proto_buff_size);

int get_nb_of_channels();
int get_samplerate();
bool get_samples(double* buffer, int buff_size);

void init_out_file(const char* path);
void write_out_file(double* buffer, double buff_size);

void free_file_handling();
void free_file_failure();

#endif
