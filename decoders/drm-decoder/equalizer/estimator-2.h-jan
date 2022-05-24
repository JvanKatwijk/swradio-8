#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J 
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__ESTIMATOR_2__
#define	__ESTIMATOR_2__

#include	<QString>
#include	"radio-constants.h"
#

//	The processor for estimating the channel(s) of a single
//	symbol
class	estimator_2 {
public:
		estimator_2	(std::complex<float> **,
	                         uint8_t, uint8_t, int16_t);
		~estimator_2	();
	void	estimate	(std::complex<float> *, std::complex<float> *);
private:
	std::complex<float>     **refFrame;
        uint8_t         Mode;
        uint8_t         Spectrum;
        int16_t         refSymbol;
        int16_t         K_min;
        int16_t         K_max;
        int16_t         indexFor        (int16_t);
        int16_t         getnrPilots     (int16_t);

	int16_t		numberofCarriers;
	int16_t		numberofPilots;
	int16_t		numberofTaps;
	int16_t		currentSymbol;
	int16_t		fftSize;
	std::complex<double>	**F_p;
	std::complex<double>	**A_p_inv;
	std::complex<float>	*pilotVector;

	std::complex<double>	**createMatrix (int, int);
	void		deleteMatrix (std::complex<double> **, int, int);
	void		multiply_d (std::complex<double> **, int, int,
	                            std::complex<double> **, int, int,
	                            std::complex<double> **);
	void		transpose (std::complex<double> **, int, int,
	                           std::complex<double> **);
	void		multiply_e (std::complex<double> **, int, int,
	                            std::complex<double> **, int, int,
	                            std::complex<double> **);
	void		multiply_f (std::complex<double> **, int, int,
	                            std::complex<double> **, int, int,
	                            std::complex<double> **);
	int16_t		*pilotTable;
};

#endif

