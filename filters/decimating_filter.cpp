
#include	"decimating_filter.h"
#include	<cstring>

	decimating_filter::decimating_filter (int inputrate,
	                                      int outrate, int freq):
	                                          fft_in (inputrate),
	                                          fft_out (outrate),
	                                          inwindow (inputrate),
	                                          freqWindow (inputrate) {
int	i;
float	f	= (float)freq / 2 / inputrate;
float	sum	= 0;
float	tmp [115];
int	degree	= 115;

	fprintf (stderr, "new filter. In: %d, out %d, freq %d\n",
	                               inputrate, outrate, freq);
	this	-> inrate	= inputrate;
	this	-> outrate	= outrate;
	inbuffer		= fft_in. getVector ();
	outbuffer		= fft_out. getVector ();
        for (i = 0; i < inrate / 2; i ++)
           inwindow [i] = 5 *  (0.42 -
                    0.5 * cos (2 * M_PI * (float)i / (float)inrate / 2) +
                    0.08 * cos (4 * M_PI * (float)i / (float)inrate / 2));
//
//	filter:
	for (i = 0; i < degree; i ++) {
           if (i == degree / 2)
              tmp [i] = 2 * M_PI * f;
           else
              tmp [i] = sin (2 * M_PI * f * (i - degree / 2))/ (i - degree / 2);
//
//      Blackman window
           tmp [i]  *= (0.42 -
                        0.5 * cos (2 * M_PI * (float)i / degree) +
                        0.08 * cos (4 * M_PI * (float)i / degree));

           sum += tmp [i];
        }

	hulpbuffer	= new std::complex<float> [inrate];
	memset (hulpbuffer, 0, inrate * sizeof (std::complex<float>));
	memset (inbuffer, 0, sizeof (std::complex<float>) * inrate);

	for (i = 0; i < degree; i ++)
	   inbuffer [i] = std::complex< float> (tmp [i] / sum,
	                                        tmp [i] / sum);

	fft_in. do_FFT ();
	for (i = 0; i < inrate; i ++) {
	   freqWindow [i] = inbuffer [i];
	}

	inp		= 0;
	outp		= outrate;
}

	decimating_filter::~decimating_filter (void) {
	fprintf (stderr, "filter being deleted\n");
}

bool decimating_filter::Pass (std::complex<float> in,
	                      std::complex<float>* out) {
bool success = false;
int	i;

	if (outp < outrate / 2) {
	   *out = outbuffer [outp];
	   outp += 1;
	   success = true;	
	}

	inbuffer   [inrate / 2 + inp] = in;
	hulpbuffer [inp] = in;

	inp ++;
	if (inp < inrate / 2) 
	   return success;

	for (i = 0; i < inrate; i ++)
	   inbuffer [i] = std::complex<float> (
	                                  real (inbuffer [i]) * inwindow [i],
	                                  imag (inbuffer [i]) * inwindow [i]);

	fft_in. do_FFT ();
	for (i = 0; i < inrate; i ++)
	   inbuffer [i] = inbuffer [i] * freqWindow [i];

	for (i = 0; i < outrate / 2; i ++) {
	   outbuffer [i] = inbuffer [i];
	   outbuffer [outrate / 2 + i] = inbuffer [inrate - outrate / 2 + i];
	}

	fft_out. do_IFFT ();
	outp	= 0;
	inp	= 0;
	for (i = 0; i < inrate / 2; i ++)
	   inbuffer [i] = hulpbuffer [i];
	return success;
}

