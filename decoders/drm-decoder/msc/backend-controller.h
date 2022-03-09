#ifndef	__BACKEND_CONTROLLER__
#define	__BACKEND_CONTROLLER__

#include	<QObject>
#include        "radio-constants.h"
#include        "basics.h"
#include	"ringbuffer.h"

class   drmDecoder;
class   stateDescriptor;
class   deInterleaver;
class   mscHandler;
class   viterbi_drm;
class	mscProcessor;

class	backendController: public QObject {
Q_OBJECT
public:
		backendController	(drmDecoder *,
	                                 RingBuffer<std::complex<float>> *,
	                                 RingBuffer<std::complex<float>> *,
	                                 int8_t);
		~backendController	();
	void	newFrame		(stateDescriptor *);
	void	addtoMux		(int16_t, int, theSignal);
	void	endofFrame		();
	void	reset			(stateDescriptor *);

private:
	drmDecoder	*drmMaster;
	RingBuffer<std::complex<float>> *iqBuffer;
	RingBuffer<std::complex<float>> *audioBuffer;
	mscProcessor	*theWorker;
	stateDescriptor	*theState;
	int8_t		qam64Roulette;
	uint8_t		mscMode;
	uint8_t		protLevelA;
	uint8_t		protLevelB;
	int16_t		numofStreams;
	uint8_t		QAMMode;
	viterbi_drm	*viterbiDecoder;
signals:
	void		showIQ		(int);
};

#endif
