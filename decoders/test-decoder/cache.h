#
//
//	to be included by the main program of the decoder

#include	<stdint.h>
#include	<complex>

class	Cache {
private:
	int	rowBound;
	int	columnBound;
	std::complex<float> **data;

public:
       Cache (int16_t a, int16_t b) {
int16_t i;

        rowBound        = a;
        columnBound     = b;
        data            = new std::complex<float> *[a];
        for (i = 0; i < a; i++)
           data [i] = new std::complex<float> [b];
	fprintf (stderr, "new cache with %d and %d\n", a, b);
}

        ~Cache (void) {
int     i;
        for (i = 0; i < rowBound; i ++)
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

