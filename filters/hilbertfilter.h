
#ifndef	__HILBERT_TRANSFORM_H
#define	__HILBERT_TRANSFORM_H
#include	<stdio.h>
#include	<fftw3.h>
#include        <complex>
#include        <cstring>

using namespace std;


class	hilbertFilter {
public:
		hilbertFilter	(int size);
		~hilbertFilter	();
std::complex<float> Pass	(float);

private:
	std::complex<float>	*fftBuffer_1;
	std::complex<float>	*fftBuffer_2;
	fftwf_plan		plan_in;
        fftwf_plan		plan_uit;
	int			size;
	int			inp;
};
#endif


