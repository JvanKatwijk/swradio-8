
//#include	<sstream>
//#include	<unoevent.h>
#include	<vector>
#include	<chrono>

#include	"drm-decoder.h"

//
//	drm specifics

#include	"drm-bandfilter.h"
#include	"drm-shifter.h"
#include	"utilities.h"

#include	"timesync.h"
#include	"freqsyncer.h"
#include	"word-collector.h"
#include	"correlator.h"
#include	"my-array.h"
#include	"referenceframe.h"
#include	"equalizer-1.h"
#include	"fac-processor.h"
#include	"fac-tables.h"
#include	"sdc-processor.h"
#include	"eqdisplay.h"
#include	"iqdisplay.h"

#define  _USE_MATH_DEFINES
#include	<math.h>

	drmDecoder::drmDecoder (RadioInterface *theRadio,
	                        RingBuffer<std::complex<float>> *audioBuffer,
	                        QSettings *s) :
	                           virtualDecoder (12000,
	                                           audioBuffer),
	                                 myFrame (nullptr),
	                                 m_worker (nullptr),
	                                 inputBuffer  (16 * 32768),
	                                 iqBuffer (32768),
	                                 eqBuffer (32768),
	                                 my_Reader (&inputBuffer,
	                                           2 * 16384, this),
#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
	                                 aacFunctions (0),
#endif
#endif
	                                 my_backendController (this, 4,
#ifdef	__WITH_FDK_AAC__
#ifdef	__MINGW32__
	                                                       &aacFunctions,
#endif
#endif
	                                                       audioBuffer,
	                                                       &iqBuffer),
	                                theState (1, 3) {
	this	-> theRadio	= theRadio;
	setupUi (&myFrame);
	my_eqDisplay            = new EQDisplay (equalizerDisplay);
	my_iqDisplay            = new IQDisplay (iqPlotter, 512);

	myFrame. show ();
	running. store (false);

	drmError		= false;
	nSymbols		= 25;
	modeInf. Mode		= 2;
	modeInf. Spectrum	= 3;

	connect (this, SIGNAL (setTimeSync (bool)),
                 this, SLOT (executeTimeSync (bool)));
        connect (this, SIGNAL (setFACSync (bool)),
                 this, SLOT (executeFACSync (bool)));
        connect (this, SIGNAL (setSDCSync (bool)),
                 this, SLOT (executeSDCSync (bool)));
        connect (this, SIGNAL (show_Mode (int)),
                 this, SLOT (execute_showMode (int)));
        connect (this, SIGNAL(show_Spectrum (int)),
                 this, SLOT (execute_showSpectrum (int)));
	connect (channel_1, SIGNAL (clicked ()),
                 this, SLOT (select_channel_1 ()));
        connect (channel_2, SIGNAL (clicked ()),
                 this, SLOT (select_channel_2 ()));

	m_worker	=
	       new std::thread (&drmDecoder::WorkerFunction, this);
}

	drmDecoder::~drmDecoder () {	
	running. store (false);
	my_Reader. stop ();
	m_worker        -> join ();
	delete m_worker;
	m_worker = nullptr;
	delete	my_eqDisplay;
	delete	my_iqDisplay;
}
//
//	Called by the underlying system
//
void	drmDecoder::
	         processBuffer (std::complex<float>* buffer, int length) {
	(void)buffer; (void)length;
}

void	drmDecoder::process (std::complex<float> v) {
	inputBuffer. putDataIntoBuffer (&v, 1);
}

void	drmDecoder::WorkerFunction () {
int16_t	blockCount      = 0;
bool	inSync;
int16_t	symbol_no       = 0;
bool	frameReady;
int counter = 0;
float     deltaFreqOffset         = 0;
float     sampleclockOffset       = 0;


	running. store (true);
	while (running. load ()) {
	   try {
	      if (!running. load ())
	         throw (21);
	      counter++;
	      emit setTimeSync (false);
	      emit setFACSync (false);
	      emit setSDCSync (false);
	      theState. cleanUp ();
	      my_Reader. waitfor (Ts_of (Mode_A));
	  
//      First step: find mode and starting point
	      modeInf. Mode = -1;
	      while (running. load () && (modeInf. Mode == -1)) {
	         my_Reader. shiftBuffer (Ts_of (Mode_A) / 3);
	         getMode (&my_Reader, &modeInf);
	      }

	      if (!running. load ())
	         throw (20);

	      setTimeSync (true);
	      my_Reader. shiftBuffer (modeInf. timeOffset_integer);
	      frequencySync (&my_Reader, &modeInf);

	      show_Mode		(modeInf. Mode);
	      show_Spectrum	(modeInf. Spectrum);
//	      show_coarseOffset	(modeInf. freqOffset_integer);
//	      show_fineOffset	(modeInf. freqOffset_fractional);

	      theState. Mode		= modeInf. Mode;
	      theState. Spectrum	= modeInf. Spectrum;
	      int nrSymbols		= symbolsperFrame (modeInf. Mode);
	      int nrCarriers       = Kmax (modeInf. Mode, modeInf. Spectrum) -
	                             Kmin (modeInf. Mode, modeInf. Spectrum) + 1;

	      myArray<std::complex<float>> inbank (nrSymbols, nrCarriers);
	      myArray<theSignal> outbank (nrSymbols, nrCarriers);
	      correlator myCorrelator (&modeInf);
	      equalizer_1 my_Equalizer (this,
	                                modeInf.Mode,
	                                modeInf.Spectrum,
	                                1,
	                                &eqBuffer);
	      std::vector<std::complex<float>> displayVector;
	      displayVector. resize (Kmax (modeInf. Mode, modeInf. Spectrum) -
	                             Kmin (modeInf. Mode, modeInf. Spectrum) + 1);
	      wordCollector my_wordCollector (this,
	                                      &my_Reader,
	                                      &modeInf,
	                                      WORKING_RATE);
	      facProcessor my_facProcessor (this, &modeInf);

//	   we know that - when starting - we are not "in sync" yet
	      inSync	= false;
//
//	   we read one full frame after which we start looking for a 
//	   match
	      for (int symbol = 0; symbol < nrSymbols - 1; symbol ++) {
	         my_wordCollector. getWord (inbank. element (symbol),
	                                    modeInf. freqOffset_integer,
	                                    modeInf. timeOffset_fractional,
	                                    modeInf. freqOffset_fractional
	                                   );
	         myCorrelator. correlate (inbank. element (symbol), symbol);
	       }

	      int  lc      = nrSymbols - 1;
//      We keep on reading here until we are satisfied that the
//      frame that is in, looks like a decent frame, just by the
//      correlation on the first word
	      while (running. load ()) {
	         my_wordCollector. getWord (inbank. element (lc),
	                                    modeInf. freqOffset_integer,
	                                    modeInf. timeOffset_fractional,
	                                    modeInf. freqOffset_fractional
	                                   );
	         myCorrelator. correlate (inbank. element (lc), lc);
	         lc = (lc + 1) % symbolsperFrame (modeInf. Mode);
	         if (myCorrelator. bestIndex (lc))  {
	            break;
	         }
	      }
		
//      from here on, we know that in the input bank, the frames occupy the
//      rows "lc" ... "(lc + symbolsinFrame) % symbolsinFrame"
//      so, once here, we know that the frame starts with index lc,
//      so let us equalize the last symbolsinFrame words in the buffer
	      for (symbol_no = 0; symbol_no < nrSymbols; symbol_no ++)
	         (void) my_Equalizer.
	            equalize (inbank. element ((lc + symbol_no) % nrSymbols),
	                      symbol_no,
	                      &outbank,
                              &modeInf. timeOffset_fractional,
                              &deltaFreqOffset,
                              &sampleclockOffset,
	                      displayVector);

	      lc           = (lc + symbol_no) % symbol_no;
	      symbol_no    = 0;
	      frameReady   = false;
	      while (running. load () && !frameReady) {
		  my_wordCollector.getWord (inbank.element(lc),
				   modeInf.freqOffset_integer,
				  lc == 0,        // no-op
				  modeInf.timeOffset_fractional,
				  deltaFreqOffset,  // tracking value
				  sampleclockOffset); // tracking value

	         frameReady = my_Equalizer. 
	                            equalize (inbank. element (lc),
	                                      symbol_no,
	                                      &outbank,
	                                      &modeInf. timeOffset_fractional,
                                              &deltaFreqOffset,
                                              &sampleclockOffset,
	                                      displayVector);
			
	         lc = (lc + 1) % nrSymbols;
	         symbol_no = (symbol_no + 1) % nrSymbols;
	      }

	      if (!running.load())
	         throw (37);

//	when we are here, we do have  our first full "frame".
//	so, we will be convinced that we are OK when we have a decent FAC
	      inSync = my_facProcessor.  processFAC  (&outbank, &theState);

	  //	one test:
	      if (!inSync)
	         throw (33);
	      if (modeInf. Spectrum != getSpectrum (&theState))
	         throw (34);
	      emit setFACSync (true);
		
//
//	prepare for sdc processing
//	Since computing the position of the sdc Cells depends (a.o)
//	on FAC and other data cells, we better create the table here.
	      sdcTable. resize (sdcCells (&modeInf));
	      set_sdcCells (&modeInf);
	      sdcProcessor my_sdcProcessor (this, &modeInf,
	                                    sdcTable, &theState);

	      bool	superframer		= false;
	      int	missers			= 0;
	      bool	firstTime		= true;
	      float	deltaFreqOffset		= 0;
	      float	sampleclockOffset	= 0;
		  
		
	      while (true) {
//	when we are here, we can start thinking about  SDC's and superframes
//	The first frame of a superframe has an SDC part
	         if (isFirstFrame (&theState)) {
	            bool sdcOK = my_sdcProcessor. processSDC (&outbank);
	            emit setSDCSync (sdcOK);
	            if (sdcOK) {
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

//	when we are here, it is time to build the next frame
	         frameReady	= false;
	         for (int i = 0; !frameReady && (i < nrSymbols); i ++) {
	            my_wordCollector.
	               getWord (inbank. element ((lc + i) % nrSymbols),
	                        modeInf. freqOffset_integer,	// initial value
	                        (lc + i) % nrSymbols == 0,
	                        modeInf. timeOffset_fractional,
	                        deltaFreqOffset,	// tracking value
	                        sampleclockOffset	// tracking value
	                      );
	            firstTime	= false;
	            frameReady =
	                  my_Equalizer.
	                         equalize (inbank. element ((lc + i) % nrSymbols),
	                                   (symbol_no + i) % nrSymbols,
	                                   &outbank,
	                                   &modeInf. timeOffset_fractional,
	                                   &deltaFreqOffset,
	                                   &sampleclockOffset,
	                                   displayVector);
	         }
	
	         if (!frameReady)	// should not happen???
	            throw (36);
			 
//	OK, let us check the FAC
	         bool success  = my_facProcessor.
	                          processFAC (&outbank, &theState);
	         if (success) {
	            emit setFACSync	(true);
	            missers = 0;
	         }
	         else {
	            emit setFACSync	(false);
	            emit setSDCSync	(false);
	            superframer		= false;
	            if (missers++ < 2)
	               continue;
	            throw (35);	// ... or give up and start all over
	         }
	      }	// end of main loop
	   } catch (int e) {
	   if (!running. load ())
	      return;
	   }
	}
}

//      just for readability
uint8_t drmDecoder::getSpectrum     (stateDescriptor *f) {
uint8_t val = f -> spectrumBits;
	return val <= 5 ? val : 3;
}
//

void	drmDecoder::getMode (Reader *my_Reader, smodeInfo *m) {
timeSyncer  my_Syncer (my_Reader, WORKING_RATE,  nSymbols);
	my_Syncer. getMode (m);
}

void    drmDecoder::frequencySync (Reader *my_Reader, smodeInfo *m) {
freqSyncer my_Syncer (my_Reader, m, WORKING_RATE, this);
	my_Syncer. frequencySync (m);
}

int16_t	drmDecoder::sdcCells (smodeInfo *m) {
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

void	drmDecoder::set_sdcCells (smodeInfo *modeInf) {
uint8_t	Mode	= modeInf -> Mode;
uint8_t	Spectrum = modeInf -> Spectrum;
int	carrier;
int	cnt	= 0;
	for (carrier = Kmin(Mode, Spectrum);
	     carrier <= Kmax(Mode, Spectrum); carrier++) {
	   if (isSDCcell (modeInf, 0, carrier)) {
		  sdcTable[cnt].symbol = 0;
		  sdcTable[cnt].carrier = carrier;
		  cnt++;
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

bool	drmDecoder::isFACcell (smodeInfo *m,
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

bool	drmDecoder::isSDCcell (smodeInfo *m,
	                             int16_t symbol, int16_t carrier) {
	if (carrier == 0)
	   return false;
	if ((m -> Mode == 1) && ((carrier == -1) || (carrier == 1)))
	   return false;

	if (symbol > 2)
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

bool	drmDecoder::isDatacell (smodeInfo *m,
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

bool	drmDecoder::isFirstFrame (stateDescriptor *f) {
uint8_t val     = f -> frameIdentity;
	return ((val & 03) == 0) || ((val & 03) == 03);
}

bool	drmDecoder::isLastFrame (stateDescriptor *f) {
uint8_t val     = f -> frameIdentity;
	return ((val & 03) == 02);
}

//
//	adding the contents of a frame to a superframe
void	drmDecoder::addtoSuperFrame (smodeInfo *m,
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

void	drmDecoder::executeTimeSync	(bool f) {
	if (f)
	   timeSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   timeSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::executeFACSync	(bool f) {
	if (f)
	   facSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   facSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void	drmDecoder::executeSDCSync	(bool f) {
	if (f)
	   sdcSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
	else {
	   sdcSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	   faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	}
}

void    drmDecoder::execute_showMode            (int l) {
        if (1 <= l && l <= 4)
           modeIndicator        -> setText (QString (char ('A' + (l - 1))));
}

void    drmDecoder::execute_showSpectrum        (int l) {
        if (0 <= l && l < 6)
           spectrumIndicator    -> setText (QString (char ('0' + l)));
}


void    drmDecoder::set_audioModeLabel	(const QString &s) {
        audioModelabel  -> setText (s);
}

void	drmDecoder::show_fineOffset	(float f) {
	show_small_offset -> display (f);
}

void	drmDecoder::show_coarseOffset	(float f) {
	show_int_offset	-> display (f);
}

void	drmDecoder::set_faadSyncLabel	(bool b) {
static  int     faadCounter     = 0;
static  int     goodfaad        = 0;
        faadCounter ++;
        if (b)
           faadSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
        else
           faadSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
        if (b)
           goodfaad ++;
        if (faadCounter > 500) {
           float ratio = goodfaad / 500.0;
           channel_4 -> setText (QString::number (ratio));
           goodfaad     = 0;
           faadCounter  = 0;
        }
}

void	drmDecoder::set_messageLabel	(const QString &s) {
	messageLabel	-> setText (s);
}

void	drmDecoder::set_timeLabel	(const QString &s) {
	timeLabel	-> setText (s);
}

void	drmDecoder::show_fac_mer	(float f) {
	fac_mer	-> display (f);;
}

void	drmDecoder::show_sdc_mer	(float f) {
	sdc_mer -> display (f);
}

void    drmDecoder::show_msc_mer        (float f) {
        msc_mer -> display (f);
}

void	drmDecoder::set_aacDataLabel	(const QString &s) {
	aacDataLabel	-> setText (s);
}

void	drmDecoder::set_channel_1	(const QString &s) {
	channel_1	-> setText (s);
	selectedService	-> setText (s);
}

void	drmDecoder::set_channel_2	(const QString &s) {
	channel_2	-> setText (s);
}

void	drmDecoder::set_channel_3	(const QString &s) {
	channel_3	-> setText (s);
}

void	drmDecoder::set_channel_4	(const QString &s) {
	channel_4	-> setText (s);
}

void	drmDecoder::set_datacoding	(const QString &s) {
	datacoding	-> setText (s);
}

void	drmDecoder::audioAvailable	() {
	audioAvailable (0, 48000);
}

void    drmDecoder::show_eqsymbol       (int amount) {
std::complex<float> line [amount];

        eqBuffer. getDataFromBuffer (line, amount);
        my_eqDisplay    -> show (line, amount);
}

void    drmDecoder::showIQ  (int amount) {
std::complex<float> Values [amount];
int16_t i;
int16_t t;
double  avg     = 0;
int     scopeWidth      = scopeSlider -> value();

	if (iqBuffer. GetRingBufferReadAvailable () < amount)
	   return;
        t = iqBuffer. getDataFromBuffer (Values, amount);
	for (i = 0; i < t; i ++) {
	   float x = abs (Values [i]);
	   if (!std::isnan (x) && !std::isinf (x))
	   avg += abs (Values [i]);
	}
	avg     /= t;
	my_iqDisplay -> DisplayIQ (Values, scopeWidth / avg);
}

void	drmDecoder::select_channel_1	() {
	theState. activate_channel_1 ();
}

void	drmDecoder::select_channel_2	() {
	theState. activate_channel_2 ();
}

