#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the drm receiver
 *
 *    drm receiver is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm receiver is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm receiver; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__ESTIMATOR_EIGEN__
#define	__ESTIMATOR_EIGEN__

#include	<Eigen/Dense>
#include	<QString>
#include	"radio-constants.h"
#include	<vector>

using namespace	Eigen;

//	The processor for estimating the channel(s) of a single
//	symbol using the "Eigen" library
//
class	estimator_2 {
public:
		estimator_2	(std::complex<float> **,
	                         uint8_t, uint8_t, int16_t);
		~estimator_2	();
	void	estimate	(std::complex<float> *,
	                         std::complex<float> *,
	                         std::vector<std::complex<float>> &);
	float	testQuality	(ourSignal *);
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
	typedef Matrix<std::complex<float>, Dynamic, Dynamic> MatrixXd;
	typedef Matrix<std::complex<float>, Dynamic, 1> Vector;
	MatrixXd	F_p;
	MatrixXd	S_p;
	MatrixXd	A_p;
	MatrixXd	A_p_inv;
	std::complex<float>	*pilotVector;
	int16_t		*pilotTable;
};

#endif

