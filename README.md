fft2osc
=======
This program resynthesizes an audiofile with a bunch of sine oscillators, after it has been analysed with a FFT.
You can also perform time-stretching and pitch-shifting operations with it.

You can compile this program with gcc or clang. The required dependencies are the development packages for fftw3 and libsndfile (and that means it can't handle mp3s).

Beware : this program was at the beginning an uni project, and is only a proof of concept.
The resynthesized file will be loaded with clicks and pops, and even if I know why, I don't really know how to implement a practical solution. 

