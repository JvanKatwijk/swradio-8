#include	"virtual-decoder.h"
#include	"radio.h"

	virtualDecoder::virtualDecoder	(int32_t workingRate,
	                                 RingBuffer<DSPCOMPLEX> *audioOut) {
//	this	-> workingRate	= workingRate;
	(void)workingRate;
	this	-> audioOut	= audioOut;
}

	virtualDecoder::~virtualDecoder	(void) {
}

void	virtualDecoder::process (std::complex<float> v) {
	(void)v;
}
