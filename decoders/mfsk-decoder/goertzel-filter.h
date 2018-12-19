#
//
//	Goertzel filter according to
//	http://netwerkt.wordpress.com/2011/08/25/goertzel-filter/
//	Wilfried Elmenreich
//
//	The filter is limited in time so we can detect
//	changes in the tones

#ifndef	__GOERTZEL__
#define	__GOERTZEL__

#include	<math.h>
#include	<stdint.h>

class	goertzelFilter {
private:
	double s_prev[2];
	double s_prev2[2];  
	double totalpower[2];  
	int	N;
	double	coeff,normalizedFreq,power,s;
	int	active;
	int	n[2];
	int	resetSamples;

public:
		goertzelFilter	(int32_t freq, int32_t samplerate);
		~goertzelFilter (void);
	float	Pass		(float sample);
};

#endif

