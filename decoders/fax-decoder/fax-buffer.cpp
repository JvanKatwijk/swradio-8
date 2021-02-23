#include	"fax-buffer.h"


	faxBuffer::faxBuffer (int samplesperLine,
		           int startFreq, int stopFreq, int nrLines):
	                            theBuffer (samplesperLine) {
	this	-> samplesperLine	= samplesperLine;
	this	-> startFreq		= startFreq;
	this	-> stopFreq		= stopFreq;
	this	-> nrLines		= nrLines;
}

	faxBuffer::~faxBuffer	() {}
	
void	faxBuffer::add	(int symbol) {
	theBuffer [bufferP] = symbol;
	bufferP = (bufferP + 1) % samplesperLine;
	nrItems ++;
}

void	faxBuffer::prepare_recognize_start	() {
	bufferP	= 0;
	nrItems	= 0;
}

bool	faxBuffer::startSignal_Detected		() {
	return false;
}

void	faxBuffer::prepare_recognize_phaseLine	() {
	bufferP	= 0;
	nrItems	= 0;
	deadline = 4 * 2 * samplesperLine;
}

bool	faxBuffer::phaseLine_Detected		() {
	if (nrItems < samplesperLine)
	   return false;

	int 	index_25	= (int) (2.5 * samplesperLine / 100);
	int	index_975	= (int) (97.5 * samplesperLine / 100);
	if (!checkWhite (0, index_25, 90))
	   return false;

	if (!checkWhite (index_975, samplesperLine - 1), 90)
	   return false;

	if (!checkBlack (index_25, index_975), 90)
	   return false;

	return true;
}

void	faxBuffer::prepare_read_phaseLine	() {
	bufferP		= 0;
	nrItems		= 0;
}

bool	faxBuffer::phaseLine_Read		() {
	if (nrItems < samplesperLine)
	   return false;

	int 	index_25	= (int) (2.5 * samplesperLine / 100);
	int	index_975	= (int) (97.5 * samplesperLine / 100);
	if (!checkWhite (0, index_25, 90))
	   return false;

	if (!checkWhite (index_975, samplesperLine - 1), 85)
	   return false;

	if (!checkBlack (index_25, index_975), 90)
	   return false;

	return true;
}

void	faxBuffer::prepare_read_imageLine	() {}

bool	faxBuffer::imageLine_Read		() {
	return false;
}


void	faxBuffer::prepare_recognize_stop	() {}

bool	faxBuffer::stopSignal_Detected		() {
	return false;
}

bool	faxBuffer::wait_too_long		(int deadLine) {
	return false;
}

	
