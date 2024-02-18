#include <iostream>
#include <fftw3.h>
#include	<complex>
#include	<cstring>

#include	"hilbertfilter.h"
using namespace std;

//	length of complex array
//#define N 64

		hilbertFilter::hilbertFilter (int size) {
	this	-> size	= size;
	fftBuffer_1	= new std::complex<float> [size];
	fftBuffer_2	= new std::complex<float> [size];
//	create a DFT plan and execute it
	plan_in = fftwf_plan_dft_1d (size,
	                        reinterpret_cast <fftwf_complex *>(fftBuffer_1),
	                        reinterpret_cast <fftwf_complex *>(fftBuffer_2),
	                        FFTW_FORWARD, FFTW_ESTIMATE);
	plan_uit = fftwf_plan_dft_1d (size,
	                        reinterpret_cast <fftwf_complex *>(fftBuffer_2),
	                        reinterpret_cast <fftwf_complex *>(fftBuffer_2),
	                        FFTW_BACKWARD, FFTW_ESTIMATE);

	for (int i = 0; i < size; i ++) {
	   fftBuffer_1 [i]	= std::complex<float> (0, 0);
	   fftBuffer_2 [i]	= std::complex<float> (0, 0);
	}
	inp			= size / 4;
}

		hilbertFilter::~hilbertFilter () {
	fftwf_destroy_plan (plan_in);
	fftwf_destroy_plan (plan_uit);
	fftwf_cleanup ();
	delete [] fftBuffer_1;
	delete [] fftBuffer_2;
}

std::complex<float> hilbertFilter::Pass (float v) {
std::complex<float> res = fftBuffer_2 [inp - size / 4];

	fftBuffer_1 [inp] = std::complex<float> (v, 0);
	inp = inp + 1;
	if (inp < this -> size)
	   return res;
//
//	OK, the buffer is full, transition starts
	fftwf_execute (plan_in);

	int hN = size >> 1; // half of the length (N/2)
	int numRem = hN; // the number of remaining elements

//	multiply the appropriate value by 2
//	(those should multiplied by 1 are left intact because
//	they wouldn't change)

	for (int i = 1; i < hN; ++i) 
	   fftBuffer_2 [i] =
	            std::complex<float> (real (fftBuffer_2 [i]) * 2,
	                                 imag (fftBuffer_2 [i]) * 2);

//	if the length is even, the number of the remaining elements decrease by 1
	if (size % 2 == 0)
	   numRem--;
	else
	if (size > 1) {	// odd and more than one element
	   fftBuffer_2 [hN] =
	                 std::complex<float> (real (fftBuffer_2 [hN]) * 2,
	                                       imag (fftBuffer_2 [hN]) * 2);
	}

//	set the negative frequencuess to 0
	for (int i = hN + 1; i < size; i ++)
	   fftBuffer_2 [i] = std::complex<float> (0, 0);
	fftwf_execute (plan_uit);
//	scale the IDFT output

	for (int i = 0; i < size; ++i) {
	   fftBuffer_2 [i] =
	             std::complex<float> (real (fftBuffer_2 [i]) / size,
	                                  imag (fftBuffer_2 [i]) / size);
	}
	for (int i = 0; i < size / 4; i ++)
	   fftBuffer_1 [i] = fftBuffer_1 [size - size / 4 - 1 + i];
	inp = size / 4;

	return res;
}

