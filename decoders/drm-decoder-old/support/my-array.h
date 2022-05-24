
#ifndef	__MY_ARRAY__
#define	__MY_ARRAY__

#include	<stdint.h>
#include	<complex.h>

template <class elementtype>
class	myArray {
public:
	myArray			(int nrows, int ncolumns) {
	this    -> rows         = nrows;
        this    -> columns      = ncolumns;
        table   = new elementtype *[rows];
        for (int i = 0; i < rows; i ++)
           table [i] = new elementtype [columns];
}

	~myArray		() {
	for (int i = 0; i < rows; i ++)
	   delete [] table [i];
	delete [] table;
}

elementtype	*element	(int row) {
	if ((row < 0) || (row >= rows))
	   throw (22);
	return table [row];
}

private:
elementtype	**table;
int	rows;
int	columns;
};

#endif

