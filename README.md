fft2osc
=======
This program resynthesizes an audiofile with a bunch of sine oscillators, after it has been analysed with a FFT.
You can also perform time-stretching and pitch-shifting operations with it.

You can compile this program with gcc or clang. The required dependencies are the development packages for fftw3 and libsndfile (meaning it can't handle mp3s).

Beware : this program was at the beginning an uni project, and is only a proof of concept. 
The resynthesized file will be loaded with clicks and pops, and even if I know why, I don't really know how to implement a practical solution. 
Moreover, it can't handle stereo and multichannel audiofiles, and it's slow when you use a relatively big FFT size.

Finally, I learned C while I was making it, and even for a small project, it's not a very maintainable one. 
The last time I tried to make the source code more bearable, I stupidly put some function declarations in the header file, as I'm not accustomed with the typical way of organizing source files in C and C++. I need to learn more about that.
I'm tempted to recode it from scratch one of these days in one of these two languages.
