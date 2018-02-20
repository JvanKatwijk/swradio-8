
#include	"decimator.h"
#include	"virtual-decoder.h"

	decimator::decimator (int32_t inRate, int32_t outRate) {
	int err;
	this	-> inRate	= inRate;
	this	-> outRate	= outRate;

	inputLimit              = inRate / 20;
        ratio                   = double(outRate) / inRate;
        outputLimit             = inputLimit * ratio;
//	src_converter		= src_new (SRC_SINC_BEST_QUALITY, 2, &err);
	src_converter		= src_new (SRC_LINEAR, 2, &err);
//	src_converter		= src_new (SRC_SINC_MEDIUM_QUALITY, 2, &err);
        src_data                = new SRC_DATA;
        inBuffer                = new float [2 * inputLimit + 20];
        outBuffer               = new float [2 * outputLimit + 20];
        src_data-> data_in      = inBuffer;
        src_data-> data_out     = outBuffer;
        src_data-> src_ratio    = ratio;
        src_data-> end_of_input = 0;
        inp                     = 0;
}

	decimator::~decimator (void) {
}

int32_t	decimator::inSize (void) {
	return inputLimit;
}

int32_t	decimator::outSize (void) {
	return outputLimit;
}

bool	decimator::add	(std::complex<float> val, std::complex<float> *b) {
int	j;
	inBuffer [2 * inp]      = real (val);
	inBuffer [2 * inp + 1]  = imag (val);
	inp ++;
	if (inp < inputLimit) 
	   return false;
	
	src_data  -> input_frames         = inp;
	src_data  -> output_frames        = outputLimit + 10;
	int res           = src_process (src_converter, src_data);
	inp               = 0;
	int framesOut       = src_data -> output_frames_gen;
	for (j = 0; j < framesOut; j ++)
	   b [j] = std::complex<float> (10 * outBuffer [2 * j],
	                                10 * outBuffer [2 * j + 1]);
	return true;
}


