#
#ifndef	__UP_FILTER_H
#define	__UP_FILTER_H

#include	<complex>
#include	<vector>
#include	<math.h>


class	upFilter {
	std::vector<std::complex<float>> kernel;
	std::vector<std::complex<float>> buffer;
	int		ip;
	int		order;
	int		bufferSize;
	int		multiplier;
public:
	upFilter	(int, int, int);
	~upFilter	();
void	Filter	(std::complex<float>, std::complex<float> *);
};
#endif

