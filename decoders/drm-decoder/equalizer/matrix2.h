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
#ifndef	__MATRIX_INVERSION_C
#define	__MATRIX_INVERSION_C

#include	<stdio.h>
#include	<stdlib.h>
#include	<complex>
#include	<math.h>
#define   TINY 1.0e-20

void	inverse		(float**,int);
int	gjinv		(float **aa, int n, float **bb);
void	inverseOfMatrix (double** M, int order);
void    multMC          (std::complex<double> **Left,
                         std::complex<double> **Right,
                         std::complex<double> ** Out, int order);

void    C_inverse       (std::complex<double> **M, int order);

#endif

