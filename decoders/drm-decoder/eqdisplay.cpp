
#include	"eqdisplay.h"


	EQDisplay::EQDisplay	(QwtPlot *plotgrid) {
	this	-> plotgrid	= plotgrid;
	plotgrid		-> setCanvasBackground (QColor ("white"));
	grid                    = new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
        grid    -> setMajPen (QPen(QColor ("black"), 0, Qt::DotLine));
#else
        grid    -> setMajorPen (QPen(QColor ("black"), 0, Qt::DotLine));
#endif
        grid    -> enableXMin (true);
        grid    -> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
        grid    -> setMinPen (QPen(QColor ("black"), 0, Qt::DotLine));
#else
        grid    -> setMinorPen (QPen(QColor ("black"), 0, Qt::DotLine));
#endif
        grid    -> attach (plotgrid);

	spectrumCurve   = new QwtPlotCurve ("");
        spectrumCurve   -> setPen (QPen(Qt::red));
        spectrumCurve   -> setOrientation (Qt::Horizontal);
        spectrumCurve   -> setBaseline  (0);
        spectrumCurve   -> attach (plotgrid);
	phaseCurve	= new QwtPlotCurve ("");
        phaseCurve	-> setPen (QPen(Qt::blue));
        phaseCurve	-> setOrientation (Qt::Horizontal);
        phaseCurve	-> setBaseline  (0);
        phaseCurve	-> attach (plotgrid);
}

	EQDisplay::~EQDisplay	() {}

void	EQDisplay::show		(std::complex<float> *v, int amount) {
double	max	= 0;
int	i;
double X_axis	[amount];
double plotData [amount];
double phaseData [amount];

	for (i = 0; i < amount; i ++) {
	   X_axis	[i] = i - amount / 2;
	   plotData	[i] = abs (v [i]);
	   if (plotData [i] > max)
	      max = plotData [i];
	}
	for (i = 0; i < amount; i ++)
	   plotData [i] = plotData [i] / max * 10;
	for (i = 0; i < amount; i ++) 
	   phaseData [i] = arg (v [i]) + 5;

	plotgrid	-> setAxisScale (QwtPlot::xBottom,
	                                 (double)X_axis [0],
	                                 (double)X_axis [amount - 1]);
	plotgrid	-> enableAxis   (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft, 0, 10);
	spectrumCurve	-> setSamples (X_axis, plotData, amount);
	phaseCurve	-> setSamples (X_axis, phaseData, amount);
	plotgrid	-> replot ();
}

