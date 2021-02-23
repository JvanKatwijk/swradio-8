
#ifndef	__CORRELATOR__
#define	__CORRELATOR__

#include	<stdint.h>
#include	<complex>
#include	<vector>
#include	"basics.h"

struct workCells {
        int16_t index;
        int16_t phase;
};

class	correlator {
public:
	correlator	(smodeInfo *s);
	~correlator	();
void	correlate	(std::complex<float> *, int);
int	maxIndex	();
void	cleanUp		();
bool	bestIndex	(int);
private:
	uint8_t	theMode;
	uint8_t	theSpectrum;
	std::vector<float>	corrTable;
	struct workCells	*refTable;
	int			Tu;
	int			K_min;
};
#endif

