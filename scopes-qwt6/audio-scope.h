#
/*
 *    Copyright (C)  2017, 2018
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *
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

#ifndef	__AUDIO_SCOPE__
#define	__AUDIO_SCOPE__

#include	"radio-constants.h"
#include	<stdexcept>
#include	"fft.h"
#include        <qwt.h>
#include        <qwt_plot.h>
#include        <qwt_plot_curve.h>
#include        <qwt_plot_grid.h>


class	audioScope {
public:
		audioScope	(QwtPlot*,	// the canvas
		                 int16_t,	// displayWidth
	                         int32_t	// samplerate
	                        );
		~audioScope	(void);
	void		addElement	(std::complex<float>);
	void		setRate		(int);
private:
	QwtPlot		*plotGrid;
	QwtPlotGrid	*grid;
	int16_t		displaySize;
	int32_t		sampleRate;
	float		*Window;
	std::complex<float>	*spectrumBuffer;
	common_fft	*spectrum_fft;
	int		averageCounter;
	int		fillPointer;
	int32_t		spectrumSize;
	double		*X_axis;
	double		*displayBuffer;
	double		*averageBuffer;
        QwtPlotCurve    *SpectrumCurve;
        QBrush          *ourBrush;
};
	
#endif

