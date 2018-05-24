

#ifndef	__DECIMATING_FILTER___
#define	__DECIMATING_FILTER__

#include	"fft.h"

class	decimating_filter {
public:
	decimating_filter	(int, int, int);
	~decimating_filter	(void);
bool	Pass		(std::complex<float>, std::complex<float> *);
private:
	int		insize;
	int		outsize;
	float		*inwindow;
	float		*freqWindow;	
	int		inp;
	int		outp;
	common_fft	*fft_in;
	common_fft	*fft_out;
	std::complex<float>	*inbuffer;
	std::complex<float>	*outbuffer;
};

#endif

