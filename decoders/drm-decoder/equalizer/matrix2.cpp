//
//	source:
//	http://bbs.dartmouth.edu/~fangq/MATH/download/source/inverse_c.htm
// inverse.c
// Source Code for "GRPP, A Scientific Programming Language 
// Processor Designed for Lex and Yacc."
// Author: James Kent Blackburn
// Goddard Space Flight Center, Code 664.0, Greenbelt, MD. 20771
// Computers in Physics, Journal Section, Jan/Feb 1994

/* 
   Matrix Inversion using 
   LU Decomposition from
   Numerical Recipes in C
   Chapter 2
*/

#include	"matrix2.h"
#include	<cstdlib>
#include	<ctime>
#define   TINY 1.0e-20

//
static
void	ludcmp		(float**, int, int*, double *);
static
void	lubksb		(float**, int, int*, double *);
void    multM		(double **Left, double **Right, double ** Out, int order);

void	multM (double **Left, double **Right, double ** Out, int order) {
	for (int row = 0; row < order; row ++) {
	   for (int col = 0; col < order; col ++) {
	      Out [row][col] = 0;
	      for (int k = 0; k < order; k ++)
	         Out [row][col] += Left [row][k] * Right [k][col];
	   }
	}
}

void	multMC (std::complex<double> **Left,
	        std::complex<double> **Right,
	        std::complex<double> ** Out, int order) {
	for (int row = 0; row < order; row ++) {
	   for (int col = 0; col < order; col ++) {
	      Out [row][col] = 0;
	      for (int k = 0; k < order; k ++)
	         Out [row][col] += Left [row][k] * Right [k][col];
	   }
	}
}

void	addTo (double **Left, double **Right, int order) {
	for (int row = 0; row < order; row ++)
	   for (int col = 0; col < order; col ++)
	      Left [row][col] += Right [row][col];
}

void	C_inverse	(std::complex<double> **M, int order) {
double **RE	= new double * [order];
double **RE_1	= new double * [order];
double **IM	= new double * [order];
double **IM_1	= new double * [order];
double **temp1	= new double * [order];
double **temp2	= new double * [order];
double **temp3	= new double * [order];

	for (int i = 0; i < order; i ++) {
	   RE     [i]	= new double [order];
	   IM     [i]	= new double [order];
	   RE_1   [i]	= new double [order];
	   IM_1   [i]	= new double [order];
	   temp1  [i]	= new double [order];
	   temp2  [i]	= new double [order];
	   temp3  [i]	= new double [order];
	}

//	step 1: split complex matrix into two real-valued ones
	for (int col = 0; col < order; col ++) {
	   for (int row = 0; row < order; row ++) {
	      RE [row][col] = real (M [row][col]);
	      RE_1 [row][col] = real (M [row][col]);
	      IM [row][col] = imag (M [row][col]);
	      IM_1 [row][col] = imag (M [row][col]);
	   }
	}

//	The real part is computed by (RE + IM * RE-1 *IM)-1

	inverseOfMatrix (RE_1, order);
	multM (IM, RE_1, temp1, order);
	multM (temp1, IM, temp2, order);
	addTo (temp2, RE, order);
	inverseOfMatrix (temp2, order);

	inverseOfMatrix (IM_1, order);
	multM (RE, IM_1, temp1, order);
	multM (temp1, RE, temp3, order);
	addTo (temp3, IM, order);
	inverseOfMatrix (temp3, order);

	for (int row = 0; row < order; row ++)
	   for (int col = 0; col < order; col ++)
	      M [row][col] = std::complex<float> (
	                             temp2 [row][col], -temp3 [row][col]);

	for (int i = 0; i < order; i ++) {
	   delete [] RE     [i];
	   delete [] IM     [i];
	   delete [] RE_1   [i];
	   delete [] IM_1   [i];
	   delete [] temp1  [i];
	   delete [] temp2  [i];
	   delete [] temp3  [i];
	}
	delete [] RE;
	delete [] RE_1; 
	delete [] IM;
	delete [] IM_1;
	delete [] temp1;
	delete [] temp2;
	delete [] temp3;
}

void	inverse (float **mat, int dim) {
int i, j, *indx;
double **y, d, *col;

	y = new double *[dim];
	for (i = 0; i < dim; i ++)
	   y [i] = new double [dim];
	indx = new int [dim];
	col = new double [dim];
	ludcmp (mat, dim, indx, &d);
	for (j = 0; j < dim; j++) {
	   for (i = 0; i < dim; i++)
	      col [i] = 0.0;
	   col [j] = 1.0;
	   lubksb (mat, dim, indx, col);
	   for (i = 0; i < dim; i++)
	      y [i][j] = col[i];
	}

	for (i = 0; i < dim; i++)
	   for (j = 0; j < dim; j++)
	      mat [i][j] = y[i][j];
	delete [] indx;
	delete [] col;
	for (i = 0; i < dim; i ++)
	   delete [] y [i];
	delete [] y;
}

void	ludcmp (float **a, int n, int *indx, double *d) {
int i, imax = 0, j, k;
double   big,dum,sum,temp;
double   *vv;

	vv = new double [n];
	*d = 1.0;
	for (i = 0; i < n; i++) {
	   big = 0.0;
	   for (j = 0; j < n; j++) {
	      if ((temp = fabs (a[i][j])) > big)
	         big = temp;
	   }

	   if (big == 0.0) {
	      fprintf (stderr,"Singular Matrix in Routine LUDCMP\n");
	      for (j = 0; j < n; j++)
	         printf (" %f ",a[i][j]);
	      printf("\n");
	      exit(1);
	   }

	   vv [i] = 1.0 / big;
	}

	for (j = 0; j < n; j++) {
	   for (i = 0; i < j; i++) {
	      sum = a[i][j];
	      for (k = 0; k < i; k++)
	         sum -= a[i][k] * a[k][j];
	      a [i][j] = sum;
	   }
	   big = 0.0;
	   for (i = j; i < n; i++) {
	      sum = a[i][j];
	      for (k = 0; k < j; k++)
	         sum -= a[i][k] * a[k][j];
	      a [i][j] = sum;
	      if ((dum = vv[i] * fabs(sum)) >= big) {
	         big = dum;
	         imax = i;
	      }
	   }
	   if (j != imax) {
	      for (k = 0; k < n; k++) {
	         dum = a [imax][k];
	         a [imax][k] = a[j][k];
	         a [j][k] = dum;
	      }

	      *d = -(*d);
	      vv [imax] = vv[j];
	   }
	   indx [j] = imax;
	   if (a [j][j] == 0.0)
	      a [j][j] = TINY;
	   if (j != n-1) {
	      dum = 1.0 / a[j][j];
	      for (i = j + 1; i < n; i++)
	         a[i][j] *= dum;
	   }
	}
	delete[] vv;
}

void lubksb (float **a, int n, int *indx, double *b) {
int i,ip,j,ii=-1;
double   sum;

	for (i = 0; i < n; i++) {
	   ip = indx [i];
	   sum = b [ip];
	   b [ip] = b [i];
	   if (ii >= 0)
	      for (j = ii; j < i; j++)
	         sum -= a[i][j] * b[j];
	   else
	   if (sum)
	      ii = i;
	   b[i] = sum;
	}
	for (i = n - 1; i >= 0; i--) {
	   sum = b[i];
	   for (j = i+1; j < n; j++)
	      sum -= a[i][j] * b[j];
	   b [i] = sum / a [i][i];
	}
}

//
//	code taken from
//	https://rosettacode.org/wiki/Gauss-Jordan_matrix_inversion#C
//
//
int gjinv (float **aa, int n, float **bb) {
int p;
long double f, g, tol;
long double a [n * n];
long double b [n * n];

	for (int row = 0; row < n; row ++) 
	   for (int col = 0; col < n; col ++) 
	      a [row * n + col] = aa [row][col];

	f = 0.;  /* Frobenius norm of a */
	for (int i = 0; i < n; i++) {
	   for (int j = 0; j < n; j++) {
	      g = a [j + i * n];
	      f += g * g;
	   }
	}
	f = sqrt (f);
	tol = f * 2.2204460492503131e-016;
	for (int i = 0; i < n; i++) {  /* Set b to identity matrix. */
	   for (int j = 0; j < n; ++j) {
	      b [j + i * n] = (i == j) ? 1. : 0.;
	   }
	}

	for (int k = 0; k < n; k ++) {  /* Main loop */
	   f = fabs (a [k + k * n]);  /* Find pivot. */
	   p = k;
	   for (int i = k + 1; i < n; i++) {
	      g = fabs (a [k + i * n]);
	      if (g > f) {
	         f = g;
	         p = i;
	      }
	   }

	   if (f < tol)
	      return 1;  /* Matrix is singular. */
	   if (p != k) {  /* Swap rows. */
	      for (int j = k; j < n; j++) {
	         f = a [j + k * n];
	         a [j + k * n] = a [j + p * n];
	         a [j + p * n] = f;
	      }

	      for (int j = 0; j < n; j++) {
	         f = b [j + k * n];
	         b [j + k * n] = b [j + p * n];
	         b [j + p * n] = f;
	      }
	   }

	   f =  a [k + k * n];  /* Scale row so pivot is 1. */
//	   f = 1. / a [k + k * n];  /* Scale row so pivot is 1. */
	   for (int j = k; j < n; j++)
	      a [j + k * n] /= f;
	   for (int j = 0; j < n; j++)
	      b [j + k * n] /= f;

	   for (int i = 0; i < n; i++) {  /* Subtract to get zeros. */
	      if (i == k)
	         continue;
	      f = a [k + i * n];
	      for (int j = k; j < n; j++)
	         a [j + i * n] -= a [j + k * n] * f;
	      for (int j = 0; j < n; j++)
	         b [j + i * n] -= b [j + k * n] * f;
	   }
	}

	for (int row = 0; row < n; row ++)
	   for (int col = 0; col < n; col ++)
	      bb [row][col] = b [row * n + col];
	return 0;
} /* end of gjinv */


// Function to perform the inverse operation on the matrix.
void	inverseOfMatrix (double** M, int order) {
double **matrix = new double * [order];
double temp;
//
//	just to be safe
	if (order < 3) { 
	   delete [] matrix;
	   return;
	}
	for (int i = 0; i < order; i ++)
	   matrix[i] = new double [2 * order];

	for (int row = 0; row < order; row ++) {
	   for (int col = 0; col < order; col ++) {
	      matrix [row][col] = M [row][col];
	      matrix [row][order + col] = 0;
	   }
	}
 
//	Create the augmented matrix
//	Add the identity matrix
//	of order at the end of original matrix.
	for (int col = 0; col < order; col++) {
//	Add '1' at the diagonal places of
//	the matrix to create a identity matrix
	   matrix [col][order + col] = 1;
	}

//	Interchange the row of matrix,
//	interchanging of row will start from the last row
	for (int i = order - 1; i > 0; i--) {
// Directly swapping the rows using pointers saves time
	   if (matrix [i - 1][0] < matrix [i][0]) {
	      double* temp = matrix [i];
	      matrix [i] = matrix [i - 1];
	      matrix [i - 1] = temp;
	   }
	}

//	Replace a row by sum of itself and a
//	constant multiple of another row of the matrix
	for (int i = 0; i < order; i++) {
	   for (int j = 0; j < order; j++) {
	      if (j != i) {
	         temp = matrix[j][i] / matrix[i][i];
	         for (int k = 0; k < 2 * order; k++) {
	             matrix[j][k] -= matrix[i][k] * temp;
	         }
	      }
	   }
	}
 
//	Multiply each row by a nonzero integer.
//	Divide row element by the diagonal element
	for (int i = 0; i < order; i++) {
	   temp = matrix[i][i];
	   for (int j = 0; j < 2 * order; j++) {
	      matrix[i][j] = matrix[i][j] / temp;
	   }
	}

	for (int row = 0; row < order; row ++)
	   for (int col = 0; col < order; col ++)
	      M [row][col] = matrix [row][order + col];

	for (int i = 0; i < order; i ++)
	   delete [] matrix [i];
	delete [] matrix;
}


