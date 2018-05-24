
#include	"decimating_filter.h"
#include	<cstring>

	decimating_filter::decimating_filter (int inrate,
	                                      int outrate, int binwidth) {
int	i;
	insize	= inrate /binwidth;
	outsize	= outrate / binwidth;
	fft_in	= new common_fft (insize);
	inbuffer = fft_in -> getVector ();
	inwindow	= new float [insize];
	fft_out = new common_fft (outsize);
	outbuffer	= fft_out -> getVector ();
	freqWindow	= new float [insize];
        for (i = 0; i < inrate; i ++)
           inwindow [i] = (0.42 -
                    0.5 * cos (2 * M_PI * (float)i / (float)insize) +
                    0.08 * cos (4 * M_PI * (float)i / (float)insize));
	freqWindow	= new float [insize];
	memset (freqWindow, 0, sizeof (float) * insize);
	for (i = 0; i < outsize; i ++) {
	   float frq = sin ((float)i / outsize * M_PI);
	   freqWindow [insize / 2 - outsize / 2 + i] = 
	                      frq * frq * frq * frq;
	}
	inp		= 0;
	outp		= outsize;
}

	decimating_filter::~decimating_filter (void) {
	   delete[]	inwindow;
	   delete[]	freqWindow;
	   delete	fft_in;
	   delete	fft_out;
	}

bool decimating_filter::Pass (std::complex<float> in,
	                            std::complex<float>* out) {
bool success = false;
int	i;

	if (outp < outsize) {
	   *out = outbuffer [outp ++];
	   success = true;
	}

	inbuffer [inp] = std::complex<float> (real (in) * inwindow [inp],
	                                      imag (in) * inwindow [inp]);
	inp ++;
	if (inp < insize) 
	   return success;

	fft_in -> do_FFT ();
	for (i = 0; i < insize; i ++)
	   inbuffer [i] = std::complex<float>
	                           (real (inbuffer [i]) *freqWindow [i],
	                            imag (inbuffer [i]) * freqWindow [i]);
	for (i = insize / 2 - outsize /2; i < insize / 2 + outsize / 2; i ++)
	   outbuffer [i - (insize / 2 - outsize / 2)] = inbuffer [i];
	fft_out -> do_IFFT ();
	return success;
}

