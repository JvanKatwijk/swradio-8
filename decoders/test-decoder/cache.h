#
//
//	to be included by the main program of the decoder

#include	<stdint.h>
#include	<complex>

class	Cache {
private:
	int	nrows;
	int	ncolums;
	std::complex<float> **data;

public:
       Cache (int16_t ncolums, int16_t nrows) {
int16_t i;

        this	-> ncolums	= ncolums;
	this	-> nrows	= nrows;
        data            = new std::complex<float> *[nrows];
        for (i = 0; i < nrows; i++)
           data [i] = new std::complex<float> [ncolums];
	fprintf (stderr, "new cache with %d rows and %d colums\n",
	                            nrows, ncolums);
}

        ~Cache (void) {
int     i;
        for (i = 0; i < nrows; i ++)
           delete[] data [i];

        delete[] data;
}

std::complex<float>     *cacheLine (int16_t n) {
        return data [n];
}

std::complex<float>	cacheElement (int16_t line, int16_t element) {
	return data [line] [element];
}
};

