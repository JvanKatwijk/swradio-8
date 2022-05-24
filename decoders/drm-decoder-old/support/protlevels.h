
#ifndef	__PROT_LEVELS__
#include	<stdint.h>
void		protLevel_qam16	(uint8_t, int16_t,
	                                           int16_t *, int16_t *);
void		protLevel_qam64	(uint8_t, int16_t,
	                                           int16_t *, int16_t *);
float		getRp_qam16	(uint8_t, int16_t);
float		getRp_qam64	(uint8_t, int16_t);
int16_t		getRYlcm_16	(int16_t);
int16_t		getRYlcm_64	(int16_t);
#endif

