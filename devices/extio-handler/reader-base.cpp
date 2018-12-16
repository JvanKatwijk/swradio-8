#
//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different here
//
//	This is the - almost empty - default implementation
#include	"reader-base.h"
#include	"extio-handler.h"

	Reader_base::Reader_base	(ExtioHandler *m, int32_t rate) {
	myMaster	= m;
	theBuffer	= m -> theBuffer;
	outputRate	= rate;
	blockSize	= -1;
	myConverter	= NULL;
	previousAmount	= 0;
}

	Reader_base::~Reader_base	(void) {
	if (myConverter != NULL) {
	   delete myConverter;
	   fprintf (stderr, "converter deleted\n");
	}
}

void	Reader_base::restartReader (int32_t s) {
	fprintf (stderr, "Restart met block %d\n", s);
	blockSize	= s;
}

void	Reader_base::stopReader	(void) {
}

void	Reader_base::processData (float IQoffs, void *data, int cnt) {
	(void)IQoffs;
	(void)data;
	(void)cnt;
}

int16_t	Reader_base::bitDepth	(void) {
	return 12;
}

void	Reader_base::setInputrate	(int32_t r) {
	inputRate	= r;
	if (myConverter != NULL)
	   delete myConverter;
	myConverter	= new converter (inputRate, outputRate, theBuffer);
}

int32_t	Reader_base::store_and_signal	(DSPCOMPLEX *buffer, int32_t amount) {
	if (myConverter == NULL)	// should not happen
	   return 0;
	(void)myConverter	-> convert (buffer, amount);
	int32_t currentAmount	= theBuffer -> GetRingBufferReadAvailable ();
	if ((previousAmount < outputRate / 10) &&
	    (currentAmount > outputRate / 10))
	   myMaster	-> putonQueue (DATA_AVAILABLE);
	previousAmount	= currentAmount;
	return currentAmount;
}

