
#ifndef	__VIRTUAL_DECODER
#define	__VIRTUAL_DECODER

#include	<QObject>
#include	"radio-constants.h"
#include	"ringbuffer.h"

class	RadioInterface;

class	virtualDecoder : public QObject {
Q_OBJECT
public:
		virtualDecoder	(int32_t,
	                         RingBuffer<std::complex<float> > *);
virtual		~virtualDecoder	(void);
virtual	void	process		(std::complex<float>);
protected:
	int32_t		workingRate;
	RingBuffer<DSPCOMPLEX> *audioOut;
signals:
	void	audioAvailable		(int, int);
	void	setDetectorMarker	(int);
	void	adjustFrequency		(int);
};
#endif

