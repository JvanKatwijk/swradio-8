#
//
//	Goertzel filter according to
//	http://netwerkt.wordpress.com/2011/08/25/goertzel-filter/
//	Wilfried Elmenreich
//
//	The filter is limited in time so we can detect
//	changes in the tones

#include	<math.h>
#include	<stdint.h>
#include	"goertzel-filter.h"


		goertzelFilter::goertzelFilter	(int32_t freq,
	                                         int32_t samplerate) {
	s_prev [0]	= s_prev [1] = 0;
	s_prev2[0]	= s_prev2[1] = 0;
	n [0]		= n [1]		= 0;
	N		= 0;
	normalizedFreq = ((double)freq) / samplerate;
	coeff = 2 * cos (2 * M_PI * normalizedFreq);
	resetSamples	= samplerate / 100;
}

		goertzelFilter::~goertzelFilter (void) {
}

float	goertzelFilter::Pass	 (float sample) {
	s = sample + coeff * s_prev[0] - s_prev2[0];
	s_prev2[0] = s_prev[0];
	s_prev[0] = s;
	n[0]++;
	s = sample + coeff * s_prev[1] - s_prev2[1];
	s_prev2[1] = s_prev[1];
	s_prev[1] = s;    
	n[1]++;
	N++;
	active = (N / resetSamples) & 0x01;
	if  (n [1-active] >= resetSamples) { // reset inactive
	   s_prev  [1-active] = 0.0;
	   s_prev2 [1-active] = 0.0;  
	   totalpower[1-active] = 0.0;  
	   n [1-active] = 0;    
	}
	totalpower[0] += sample*sample;
	totalpower[1] += sample*sample;
	power = s_prev2[active] * s_prev2[active]
	        + s_prev[active] * s_prev[active]
	        - coeff * s_prev[active] * s_prev2[active];
	return power / (totalpower[active]+1e-7) / n[active];
}


