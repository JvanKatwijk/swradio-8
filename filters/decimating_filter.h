

#ifndef	__DECIMATING_FILTER__
#define	__DECIMATING_FILTER__

#include	"fft.h"
#include	<vector>

class	decimating_filter {
public:
	decimating_filter	(int, int, int);
	~decimating_filter	(void);
bool	Pass		(std::complex<float>, std::complex<float> *);
private:
	int		inrate;
	int		outrate;
	common_fft	fft_in;
	common_ifft	fft_out;
	std::vector<float>	inwindow;
	std::vector<std::complex<float>>	freqWindow;	
	std::complex<float>	*hulpbuffer;
	int		inp;
	int		outp;
	std::complex<float>	*inbuffer;
	std::complex<float>	*outbuffer;
};

#endif

