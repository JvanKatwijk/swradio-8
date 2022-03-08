
#include	"up-converter.h"

	audioConverter::audioConverter (int32_t inRate, int32_t outRate,
	                          int inSize) {
	this	-> inRate	= inRate;
	this	-> outRate	= outRate;
	(void)inSize;

	fprintf (stderr, "Going for a %d converter\n", inRate);
	switch (inRate) {
	   case 9600:
	      workerHandle	= new converter_9600 ();
	      break;

	   case 12000:
	      workerHandle	= new converter_12000 ();
	      break;

	   case 16000:
	      workerHandle	= new converter_16000 ();
	      break;

	   case 19200:
	      workerHandle	= new converter_19200 ();
	      break;

	   case 24000:
	      workerHandle	= new converter_24000 ();
	      break;

	   case 32000:
	      workerHandle	= new converter_32000 ();
	      break;

	   case 38400:
	      workerHandle	= new converter_38400 ();
	      break;

	   case 48000:
	      workerHandle	= new converter_48000 ();
	      break;

	   default:
	      workerHandle	= new converter_base	();
	      break;
	}
}

	audioConverter::~audioConverter	() {
	if (workerHandle != nullptr)
	   delete workerHandle;
}

bool	audioConverter::convert (std::complex<float> v,
	                      std::complex<float> *out, int *amount) {
	return workerHandle -> convert (v, out, amount);
}

int	audioConverter::getOutputSize	() {
	return workerHandle -> getOutputSize ();
}

	converter_base::converter_base	() {
}

	converter_base::~converter_base	() {
}

bool	converter_base::convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount) {
	(void)v; (void)out;
	*amount	= 0;
	return false;
}

int	converter_base::getOutputSize	() {
	return 0;
}

	converter_9600::converter_9600	():
	                                theFilter (15, 4800, 48000) {
	buffer. resize (960);
	inP	= 0;
}

	converter_9600::~converter_9600 () {}

bool	converter_9600::convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int * amount) {
	out [inP ++] = v;
	if (inP < 960) {
	   *amount = 0;
	   return false;
	}

	int outP = 0;
	for (int i = 0; i < 960; i ++) {
	   out [outP ++] = theFilter. Pass (v);
	   out [outP ++] = theFilter. Pass (std::complex<float> (0, 0));
	   out [outP ++] = theFilter. Pass (std::complex<float> (0, 0));
	   out [outP ++] = theFilter. Pass (std::complex<float> (0, 0));
	   out [outP ++] = theFilter. Pass (std::complex<float> (0, 0));
	}
	*amount = 4800;
	inP	= 0;
	return true;
}

int	converter_9600::getOutputSize	() {
	return 4800;
}

	converter_12000::converter_12000	():
	                                theFilter (15, 6000, 48000) {
	buffer. resize (1200);
	inP	= 0;
}

	converter_12000::~converter_12000 () {}

bool	converter_12000::convert	(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int * amount) {
	buffer [inP ++] = v;
	if (inP < 1200) {
	   *amount = 0;
	   return false;
	}

	int outP = 0;
	for (int i = 0; i < 1200; i ++) {
	   out [outP ++] = theFilter. Pass (buffer [i]);
	   out [outP ++] = theFilter. Pass (std::complex<float> (0, 0));
	   out [outP ++] = theFilter. Pass (std::complex<float> (0, 0));
	   out [outP ++] = theFilter. Pass (std::complex<float> (0, 0));
	}
	*amount = 4800;
	inP	= 0;
	return true;
}

int	converter_12000::getOutputSize	() {
	return 4800;
}
//
	converter_16000::converter_16000	():
	                                theFilter (15, 8000, 48000) {
	buffer. resize (1600);
	inP	= 0;
}

	converter_16000::~converter_16000 () {}

bool	converter_16000::convert	(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int * amount) {
	out [inP++] = v;
	if (inP < 1600) {
	   *amount = 0;
	   return false;
	}

	int	outP	= 0;
	for (int i = 0; i < 1600; i ++) {
	   out [i] = theFilter. Pass (v);
	   out [i] = theFilter. Pass (std::complex<float> (0, 0));
	   out [i] = theFilter. Pass (std::complex<float> (0, 0));
	}
	*amount = 4800;
	inP	= 0;
	return true;
}

int	converter_16000::getOutputSize	() {
	return 4800;
}
//
//
//	conversion from 19200 -> 48000 is 
//	19200 -> 192000 -> 48000, i.e. 200 samples in, 500 out
//
	converter_19200::converter_19200	():
	                                theFilter (15, 9600, 96000) {
	buffer. resize (2000);
	inP	= 0;
}

	converter_19200::~converter_19200 () {}

bool	converter_19200::convert	(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount) {
	buffer [inP ++] = theFilter. Pass (v);
	for (int i = 1; i < 10; i ++) {
	   buffer [inP] = theFilter. Pass (std::complex<float> (0, 0));
	   inP = inP  + 1;
	}

	if (inP < 2000) {
	   *amount = 0;
	   return false;
	}

	for (int i = 0; i < 500; i ++) 
	   out [i] = buffer [4 * i];
	*amount = 500;
	inP	= 0;
	return true;
}

int	converter_19200::getOutputSize	() {
	return 500;
}

//	conversion from 24000 -> 48000 is 
//	24000 -> 48000, i.e. 1 in, 2 out
//
	converter_24000::converter_24000	():
	                                theFilter (15, 12000, 48000) {
	buffer. resize (4800);
	inP = 0;
}

	converter_24000::~converter_24000 () {}

bool	converter_24000::convert	(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount) {
	buffer [inP ++] = theFilter. Pass (v);
	buffer [inP ++] = theFilter. Pass (std::complex<float> (0, 0));
	if (inP < 4800) {
	   *amount = 0;
	   return false;
	}
	*amount = 4800;
	for (int i = 0; i < 4800; i ++)
	   out [i] = buffer [i];
	inP	= 0;
	return true;
}

int	converter_24000::getOutputSize	() {
	return 4800;
}
//	conversion from 32000 -> 48000 is 
//	32000 -> 96000 -> 48000, i.e. 200 in 300 out
//
	converter_32000::converter_32000	():
	                                theFilter (15, 16000, 96000) {
	buffer. resize (1200);
	inP	= 0;
}
	
	converter_32000::~converter_32000 () {}

bool	converter_32000::convert	(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount) {
	buffer [inP ++] = theFilter. Pass (v);
	buffer [inP ++] = theFilter. Pass (std::complex<float> (0, 0));
	buffer [inP ++] = theFilter. Pass (std::complex<float> (0, 0));
	if (inP < 1200) {
	   *amount = 0;
	   return false;
	}
	for (int i = 0; i < 600; i ++)
	   out [i] = buffer [2 * i];
	inP		= 0;
	*amount = 600;
	return true;
}

int	converter_32000::getOutputSize	() {
	return 600;
}

//	conversion from 38400 -> 48000 is 
//	384000 -> 192000 -> 48000, i.e. 4 in 5 out
//
	converter_38400::converter_38400	():
	                                theFilter (15, 19200, 192000) {
	buffer. resize (2000);
	inP	= 0;
}
	
	converter_38400::~converter_38400 () {}

bool	converter_38400::convert	(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount) {
	buffer [inP ++] = theFilter. Pass (v);
	for (int i = 0; i < 5; i ++)
	    buffer [inP ++] = theFilter. Pass (std::complex<float> (0, 0));
	if (inP < 2000) {
	   *amount = 0;
	   return false;
	}
	for (int i = 0; i < 500; i ++)
	   out [i] = buffer [4 * i];
	*amount = 500;
	inP	= 0;
	return true;
}

int	converter_38400::getOutputSize	() {
	return 500;
}

	converter_48000::converter_48000 ():
	                                   theFilter (15, 22000, 48000) {
	buffer. resize (2400);
	inP	= 0;
}

	converter_48000::~converter_48000 () { 
}

bool	converter_48000::convert	(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount) {

	buffer [inP++] = theFilter. Pass (v);
	if (inP < 2400) {
	   *amount = 0;
	   return false;
	}

	for (int i = 0; i < 2400; i ++)
	   out [i] = buffer [i];
	*amount = 2400;
	inP	= 0;
	return true;
}

int	converter_48000::getOutputSize	() {
	return 2400;
}

