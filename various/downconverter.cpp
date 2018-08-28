
#include	"downconverter.h"
#include	"fft-filters.h"

#define FILTER_SIZE     1024
#define FILTER_STRENGTH  255


	downConverter::downConverter (int32_t inRate, int32_t outRate) {
int	err;
	this	-> inRate	= inRate;
	this	-> outRate	= outRate;
	filtering_needed	= inRate > outRate;
	if (filtering_needed) {
	   this	-> hfFilter	= new fftFilter (FILTER_SIZE,
                                                 FILTER_STRENGTH);
           hfFilter-> setBand (-outRate / 2, outRate / 2, inRate);
	}
	else
	   this	-> hfFilter	= NULL;
	inputLimit              = 2048;
        ratio                   = double(outRate) / inRate;
        outputLimit             = inputLimit * ratio;
//	src_converter		= src_new (SRC_SINC_BEST_QUALITY, 2, &err);
//	src_converter		= src_new (SRC_LINEAR, 2, &err);
	src_converter		= src_new (SRC_SINC_MEDIUM_QUALITY, 2, &err);
	if (src_converter == NULL)
	   fprintf (stderr, "van %d naar %d ging niet\n", inRate, outRate);
	else
	   fprintf (stderr, "van %d naar %d gaat wel\n", inRate, outRate);
        src_data                = new SRC_DATA;
        inBuffer                = new float [2 * inputLimit + 20];
        outBuffer               = new float [2 * outputLimit + 20];
        src_data-> data_in      = inBuffer;
        src_data-> data_out     = outBuffer;
        src_data-> src_ratio    = ratio;
        src_data-> end_of_input = 0;
        inp                     = 0;
	dataBuffer		= new RingBuffer<DSPCOMPLEX> (1024 * 1024);
}

	downConverter::~downConverter	(void) {
	if (hfFilter != NULL)
	   delete hfFilter;
	delete[]	inBuffer;
	delete[]	outBuffer;
	delete		dataBuffer;
	src_delete (src_converter);
	delete		src_data;
}

int32_t	downConverter::bufferSize_in	(void) {
	return inputLimit;
}

int32_t	downConverter::bufferSize_out	(void) {
	return outputLimit;
}

void	downConverter::convert_in	(DSPCOMPLEX *buffer) {
int32_t	i;

	for (i = 0; i < inputLimit; i ++) {
	   if (filtering_needed) {
	      DSPCOMPLEX tmp = hfFilter -> Pass (buffer [i]);
	      inBuffer [2 * inp]      = real (tmp);
	      inBuffer [2 * inp + 1]  = imag (tmp);
	   }
	   else {
	      inBuffer [2 * inp]      = real (buffer [i]);
	      inBuffer [2 * inp + 1]  = imag (buffer [i]);
	   }
	   inp ++;
	   if (inp >= inputLimit) {
	      src_data	-> input_frames         = inp;
	      src_data  -> output_frames        = outputLimit + 10;
	      int res       = src_process (src_converter, src_data);
	      inp       = 0;
	      int framesOut       = src_data -> output_frames_gen;
	      DSPCOMPLEX b2 [framesOut];
	      for (int j = 0; j < framesOut; j ++)
	         b2 [j] = DSPCOMPLEX (outBuffer [2 * j] * 10,
	                              outBuffer [2 * j + 1]* 10);
	      dataBuffer -> putDataIntoBuffer (b2, framesOut);
	   }
	}
}

uint32_t downConverter::hasData	(void) {
	return dataBuffer -> GetRingBufferReadAvailable ();
}

int32_t	downConverter::dataOut	(DSPCOMPLEX *buffer, int amount) {
	return dataBuffer -> getDataFromBuffer (buffer, amount);
}

