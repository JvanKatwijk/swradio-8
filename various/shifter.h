

#ifndef	__SHIFTER__
#define	__SHIFTER__
#include	"radio-constants.h"

class	shifter {
public:
			shifter		(int32_t);
			~shifter	(void);
	std::complex<float>	do_shift (std::complex<float>, int32_t);
	void		do_shift	(std::complex<float> *,
	                                 int32_t, int32_t);
private:
	int32_t		phase;
	int32_t		tableSize;
	std::complex<float>	*table;
};
#endif

