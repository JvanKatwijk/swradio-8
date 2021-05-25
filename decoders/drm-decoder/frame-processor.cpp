#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of drm receiver
 *
 *    drm receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm receiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm receiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"drm-decoder.h"
#include	"frame-processor.h"
#include	"state-descriptor.h"
#include	"fac-processor.h"
#include	"sdc-processor.h"
#include	"msc-processor.h"
#include	"freqsyncer.h"
#include	"referenceframe.h"
#include	"basics.h"
#include	"fac-tables.h"
#include	"my-array.h"
#include	"correlator.h"

//	The main driving class is the frameprocessor. It is
//	implemented as running in a thread and will control:
//	a. initial synchronization and resynchronization after sync failures
//	b. getting synchronized data for the frames
//	b. assembling the FAC and SDC,
//	c. collecting Muxes and passing the Mux-es to the  MSC handler
//
//	windowDepth and qam64Roulette are just there to
//	allow some fidling - without recompilation - with the
//	size of the window used for equalization and the
//	number of iterations for the multi-level decoding
	frameProcessor::frameProcessor (drmDecoder	*mr,
	                                RingBuffer<std::complex<float>> *buffer,
	                                RingBuffer<std::complex<float>> *iqBuffer,
	                                RingBuffer<std::complex<float>> *eqBuffer,
	                                int32_t		sampleRate,
	                                int16_t		nSymbols,
	                                int8_t		windowDepth,
                                        int8_t 		qam64Roulette) :
	                                 my_Reader (buffer, 4 * 16384, mr),
	                                 my_backendController (mr,
	                                                       iqBuffer,
	                                                       qam64Roulette),
	                                 theState (mr, 1, 3) {
	this	-> theDecoder	= mr;
	this	-> buffer	= buffer;
	this	-> iqBuffer	= iqBuffer;
	this	-> eqBuffer	= eqBuffer;
	this	-> sampleRate	= sampleRate;
	this	-> nSymbols	= nSymbols;
	this	-> windowDepth	= windowDepth;
//
//	defaults, will be overruled almost immediately
	modeInf. Mode		= 1;
	modeInf. Spectrum	= 3;

	connect (this, SIGNAL (setTimeSync (bool)),
	         theDecoder, SLOT (executeTimeSync (bool)));
	connect (this, SIGNAL (setFACSync (bool)),
	         theDecoder, SLOT (executeFACSync (bool)));
	connect (this, SIGNAL (setSDCSync (bool)),
	         theDecoder, SLOT (executeSDCSync (bool)));
	connect (this, SIGNAL (show_Mode (int)),
	         theDecoder, SLOT (execute_showMode (int)));
	connect (this, SIGNAL(show_Spectrum (int)),
	         theDecoder, SLOT (execute_showSpectrum (int)));
	connect (this, SIGNAL (show_services (int, int)),
	         theDecoder, SLOT (show_channels (int, int)));
	connect (this, SIGNAL (show_audioMode (QString)),
	         theDecoder, SLOT (show_audioMode (QString)));
//	connect (this, SIGNAL (showSNR (float)),
//	         theDecoder, SLOT (showSNR (float)));
}

	frameProcessor::~frameProcessor (void) {
	my_Reader. stop ();
	while (isRunning ())
	   usleep (100);
}

void	frameProcessor::stop (void) {
	my_Reader. stop ();		// will raise a signal
}

//
//	The frameprocessor is the driving force. It will continuously
//	read in (ofdm) symbols, build frames and send frames to the
//	various processors (performing some validity checks as part of the
//	process)
void	frameProcessor::run	(void) {
int16_t		blockCount	= 0;
bool		inSync;
bool		superframer	= false;
int16_t		threeinaRow;
int16_t		symbol_no	= 0;
bool		frameReady;
int16_t		missers;
timeSyncer  my_Syncer (&my_Reader, sampleRate,  nSymbols);
//
	try {
restart:

	   setTimeSync		(false);
	   setFACSync		(false);
	   setSDCSync		(false);
	   show_services	(0, 0);
	   bool superframer	= false;	
	   int threeinaRow	= 0;
	   show_audioMode	(QString ("  "));
	   int 	missers		= 0;

	   fprintf (stderr, "decoder starts\n");
//	First step: find mode and starting point
	   modeInf. Mode = -1;
	   while (modeInf. Mode == -1) {
	      my_Reader. shiftBuffer (Ts_of (Mode_A) / 2);
	      getMode (&my_Reader, &modeInf);
           }

	   fprintf (stderr, "found: Mode = %d, time_offset = %d, sampleoff = %f freqoff = %f\n",
	        modeInf. Mode,
	        modeInf. timeOffset_integer,
	        modeInf. sampleRate_offset,
	        modeInf. freqOffset_fract);

	   if ((modeInf. Spectrum < 1) || (modeInf. Spectrum > 3))
	      modeInf. Spectrum = 3;

//	adjust the bufferpointer:
	   my_Reader. shiftBuffer (modeInf. timeOffset_integer);

//	and create a reader/processor
	   frequencySync (theDecoder, &my_Reader, &modeInf);

//	   fprintf (stderr, "spectrum is gezet op %d\n", modeInf. Spectrum);
	   setTimeSync	(true);
	   show_Mode (modeInf. Mode);
	   show_Spectrum (modeInf. Spectrum);
//
	   theState. Mode	= modeInf. Mode;
	   theState. Spectrum	= modeInf. Spectrum;
	   int nrSymbols	= symbolsperFrame (modeInf. Mode);
	   int nrCarriers	= Kmax (modeInf. Mode, modeInf. Spectrum) -
	                          Kmin (modeInf. Mode, modeInf. Spectrum) + 1;

	   myArray<std::complex<float>> inbank (nrSymbols, nrCarriers);
	   myArray<theSignal> outbank (nrSymbols, nrCarriers);
	   correlator myCorrelator (&modeInf);
	   equalizer_1 my_Equalizer (theDecoder,
	                             modeInf. Mode,
	                             modeInf. Spectrum,
	                             windowDepth,
	                             eqBuffer);
	   wordCollector my_wordCollector (theDecoder,
	                                   &my_Reader,
	                                   &modeInf,
	                                   sampleRate);
	   facProcessor my_facProcessor	(theDecoder, &modeInf);
//
//
//	we know that - when starting - we are not "in sync" yet
	   inSync		= false;

	   for (int symbol = 0; symbol < nrSymbols; symbol ++) {
	      my_wordCollector. getWord (inbank. element (symbol),
	                                 modeInf. freqOffset_integer,
	                                 modeInf. timeOffset_fractional);
	      myCorrelator. correlate (inbank. element (symbol), symbol);
	   }

	   int16_t errors	= 0;
	   int	lc	= 0;
//	We keep on reading here until we are satisfied that the
//	frame that is in, is indeed a valid, synchronized frame

	   while (true) {
	      my_wordCollector. getWord (inbank. element (lc),
	                                 modeInf. freqOffset_integer,
	                                 modeInf. timeOffset_fractional);
	      myCorrelator. correlate (inbank. element (lc), lc);
	      lc = (lc + 1) % symbolsperFrame (modeInf. Mode);
	      if (myCorrelator. bestIndex (lc))  {
	         break;
	      }
	   }
//
//	from here on, we know that in the input bank, the frames occupy the
//	rows "lc" ... "(lc + symbolsinFrame) % symbolsinFrame"
//	so, once here, we know that the frame starts with index lc,
//	so let us equalize the last symbolsinFrame words in the buffer
	   for (symbol_no = 0; symbol_no < nrSymbols; symbol_no ++) 
	      (void) my_Equalizer.
	            equalize (inbank. element ((lc + symbol_no) % nrSymbols),
	                                               symbol_no, &outbank);

	   lc		= (lc + symbol_no) % symbol_no;
	   symbol_no	= 0;
	   frameReady	= false;
	   while (!frameReady) {
	      my_wordCollector.  getWord (inbank. element (lc),
	                                  modeInf. freqOffset_integer,
	                                  modeInf. timeOffset_fractional);
	      frameReady = my_Equalizer.  equalize (inbank. element (lc),
                                                    symbol_no,
                                                    &outbank);

	      lc = (lc + 1) % nrSymbols;
	      symbol_no = (symbol_no + 1) % nrSymbols;
	   }

//	when we are here, we do have a full "frame".
//	so, we will be convinced that we are OK when we have a decent FAC
	   inSync = my_facProcessor. 
	                  processFAC  (my_Equalizer. getMeanEnergy (),
	                               my_Equalizer. getChannels   (),
	                               &outbank, &theState);
//	one test:
	   if (modeInf. Spectrum != getSpectrum (&theState))
	      goto restart;

	   if ((!inSync) && (++errors > 5))
	      goto restart;

//
//	prepare for sdc processing
//	Since computing the position of the sdc Cells depends (a.o)
//	on FAC and other data cells, we better create the table here.
	   sdcTable. resize (sdcCells (&modeInf));
	   set_sdcCells (&modeInf);
	   sdcProcessor my_sdcProcessor (theDecoder, &modeInf,
	                                 &sdcTable, &theState);

	   bool	firstTime	= true;
	   float	offsetFractional	= 0;	//
	   int16_t	offsetInteger		= 0;
	   float	deltaFreqOffset		= 0;
	   float	sampleclockOffset	= 0;
	   while (true) {
	      setFACSync (true);
//	when we are here, we can start thinking about  SDC's and superframes
//	The first frame of a superframe has an SDC part
	      if (isFirstFrame (&theState)) {
	         bool sdcOK = my_sdcProcessor. processSDC (&outbank);
	         setSDCSync (sdcOK);
	         if (sdcOK) {
	            show_services (getnrAudio (&theState),
	                        getnrData (&theState));
	            blockCount	= 0;
	         }
//
//	if we seem to have the start of a superframe, we
//	re-create a backend with the right parameters
	         if (!superframer && sdcOK)
	            my_backendController. reset (&theState);
	         superframer	= sdcOK;
	      }
//
//	when here, add the current frame to the superframe.
//	Obviously, we cannot garantee that all data is in order
	      if (superframer)
	         addtoSuperFrame (&modeInf, blockCount ++, &outbank);
//
//	when we are here, it is time to build the next frame
	      frameReady	= false;
	      for (int i = 0;
	           !frameReady && (i < nrSymbols); i ++) {
	         my_wordCollector.
	              getWord (inbank. element ((lc + i) % nrSymbols),
	                       modeInf. freqOffset_integer,	// initial value
	                       firstTime,
	                       modeInf. timeOffset_fractional,
	                       deltaFreqOffset,		// tracking value
	                       sampleclockOffset	// tracking value
	                      );
	         firstTime = false;

	         frameReady =
	                  my_Equalizer.
	                         equalize (inbank. element ((lc + i) % nrSymbols),
	                                   (symbol_no + i) % nrSymbols,
	                                   &outbank,
	                                   &modeInf. timeOffset_fractional,
	                                   &deltaFreqOffset,
	                                   &sampleclockOffset);
	      }

//	OK, let us check the FAC
	      bool success  = my_facProcessor.
	                          processFAC (my_Equalizer. getMeanEnergy (),
	                                      my_Equalizer. getChannels   (),
	                                      &outbank, &theState);
	      if (success) 
	         missers = 0;
	      else {
	         setFACSync (false); setSDCSync (false);
	         superframer		= false;
	         if (missers++ < 2)
	            continue;
	         goto restart;	// ... or give up and start all over
	      }
	   }	// end of main loop
	}
	catch (int e) {
	   fprintf (stderr, "frameprocessor will halt with %d\n", e);
	}
}
//
//	adding the contents of a frame to a superframe
void	frameProcessor::addtoSuperFrame (smodeInfo *m,
	                                 int16_t blockno, 
	                                 myArray<theSignal> *outbank) {
static	int	teller	= 0;
int16_t	symbol, carrier;
int16_t	   K_min		= Kmin (m -> Mode, m -> Spectrum);
int16_t	   K_max		= Kmax (m -> Mode, m -> Spectrum);

	if (isFirstFrame (&theState))
	   my_backendController. newFrame (&theState);

	for (symbol = 0; symbol < symbolsperFrame (m -> Mode); symbol ++) {
	   for (carrier = K_min; carrier <= K_max; carrier ++)
	      if (isDatacell (&modeInf, symbol, carrier, blockno)) {
	         my_backendController. addtoMux (blockno, teller ++,
	                                     outbank -> element (symbol)[carrier - K_min]);
	      }
	}

	if (isLastFrame (&theState)) {
	   my_backendController. endofFrame ();
	   teller = 0;
	}
}

bool	frameProcessor::isLastFrame (stateDescriptor *f) {
uint8_t	val	= f -> frameIdentity;
	return ((val & 03) == 02);
}

bool	frameProcessor::isFirstFrame (stateDescriptor *f) {
uint8_t	val	= f -> frameIdentity;
	return ((val & 03) == 0) || ((val & 03) == 03);
}

bool	frameProcessor::isDatacell (smodeInfo *m,
	                            int16_t symbol,
	                            int16_t carrier, int16_t blockno) {
	if (carrier == 0)
	   return false;
	if (m -> Mode == 1 && (carrier == -1 || carrier == 1))
	   return false;
//
//	these are definitely SDC cells
	if ((blockno == 0) && ((symbol == 0) || (symbol == 1)))
	   return false;
//
	if ((blockno == 0) && ((m -> Mode == 3 ) || (m -> Mode == 4)))
	   if (symbol == 2)
	      return false;
	if (isFreqCell (m -> Mode, symbol, carrier))
	   return false;
	if (isPilotCell (m -> Mode, symbol, carrier))
	   return false;
	if (isTimeCell (m -> Mode, symbol, carrier))
	   return false;
	if (isFACcell (m, symbol, carrier))
	   return false;

	return true;
}

int16_t	frameProcessor::sdcCells (smodeInfo *m) {
static
int m1_table []	= {167, 190, 359, 405, 754, 846};
static
int m2_table [] = {130, 150, 282, 322, 588, 1500};

	switch (m -> Mode) {
	   case Mode_A:
	      return m1_table [m -> Spectrum];

	   default:
	   case Mode_B:
	      return m2_table [m -> Spectrum];

	   case Mode_C:
	      return 288;

	   case Mode_D:
	      return 152;
	}
	return 288;
}

bool	frameProcessor::isFACcell (smodeInfo *m,
	                           int16_t symbol, int16_t carrier) {
int16_t	i;
struct facElement *facTable     = getFacTableforMode (m -> Mode);

//	we know that FAC cells are always in positive carriers
	if (carrier < 0)
	   return false;
	for (i = 0; facTable [i]. symbol != -1; i ++) {
	   if (facTable [i]. symbol > symbol)
	      return false;
	   if ((facTable [i]. symbol == symbol) &&
	       (facTable [i]. carrier == carrier))
	      return true;
	}
	return false;
}

bool	frameProcessor::isSDCcell (smodeInfo *m,
	                           int16_t symbol, int16_t carrier) {
	if (carrier == 0)
	   return false;
	if ((m -> Mode == 1) && ((carrier == -1) || (carrier == 1)))
	   return false;

	if (isTimeCell (m -> Mode, symbol, carrier))
	   return false;
	if (isFreqCell (m -> Mode, symbol, carrier))
	   return false;
	if (isPilotCell (m -> Mode, symbol, carrier))
	   return false;
	if (isFACcell (m, symbol, carrier))
	   return false;
	return true;
}

void	frameProcessor::set_sdcCells (smodeInfo *modeInf) {
uint8_t	Mode	= modeInf -> Mode;
uint8_t	Spectrum = modeInf -> Spectrum;
int	carrier;
int	cnt	= 0;
	for (carrier = Kmin (Mode, Spectrum);
	     carrier <= Kmax (Mode, Spectrum); carrier ++) {
           if (isSDCcell (modeInf, 0, carrier)) {
	      sdcTable [cnt]. symbol = 0;
	      sdcTable [cnt]. carrier = carrier;
	      cnt ++;
	   }
	}

        for (carrier = Kmin (Mode, Spectrum);
             carrier <= Kmax (Mode, Spectrum); carrier ++) {
           if (isSDCcell (modeInf, 1, carrier)) {
	      sdcTable [cnt]. symbol = 1;
	      sdcTable [cnt]. carrier = carrier;
	      cnt ++;
	   }
	}
	   
	if ((Mode == Mode_C) || (Mode == Mode_D)) {
	   for (carrier = Kmin (Mode, Spectrum);
	        carrier <= Kmax (Mode, Spectrum); carrier ++) 
	      if (isSDCcell (modeInf, 2, carrier)) {
	         sdcTable [cnt]. symbol = 2;
	         sdcTable [cnt]. carrier = carrier;
	         cnt ++;
	      }
	}

	fprintf (stderr, "for Mode %d, spectrum %d, we have %d sdc cells\n",
	                       Mode, Spectrum, cnt);
}
//
//	just for readability
uint8_t	frameProcessor::getSpectrum	(stateDescriptor *f) {
uint8_t val = f -> spectrumBits;
	return val <= 5 ? val : 3;
}
//

void    frameProcessor::getMode (Reader *my_Reader, smodeInfo *m) {
timeSyncer  my_Syncer (my_Reader, sampleRate,  nSymbols);
        my_Syncer. getMode (m);
}

void    frameProcessor::frequencySync (drmDecoder *mr,
                                       Reader *my_Reader, smodeInfo *m) {
freqSyncer my_Syncer (my_Reader, m, sampleRate, mr);
	my_Syncer. frequencySync (m);
}

int16_t	frameProcessor::getnrAudio	(stateDescriptor *theState) {
int16_t i;
int16_t	amount	= 0;

	for (i = 0; i < theState -> numofStreams; i ++)
	   if (theState -> streams [i]. soort == stateDescriptor::AUDIO_STREAM)
	      amount ++;
	return amount;
}

int16_t	frameProcessor::getnrData	(stateDescriptor *theState) {
int16_t i;
int16_t	amount	= 0;

	for (i = 0; i < theState ->  numofStreams; i ++)
	   if (theState -> streams [i]. soort == stateDescriptor::DATA_STREAM)
	      amount ++;
	return amount;
}

