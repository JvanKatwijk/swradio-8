
#include	"bandplan.h"

	bandPlan::bandPlan (QString fileName) {
	labelTable. resize (0);
	loadPlan (fileName);
	
}

	bandPlan::~bandPlan () {
}

const
QString	bandPlan::getFrequencyLabel (uint32_t Frequency) {
int	freq	= Frequency / 1000;

	for (int i = 0; i < labelTable. size (); i ++)
	   if ((labelTable. at (i). low <= freq) &&
	       (freq <= labelTable. at (i). high))
	      return QString (labelTable. at (i). label);

	return QString ("");
}


bool	bandPlan::loadPlan (QString fileName) {
QDomDocument xmlBOM;
QFile f (fileName);

	this	-> fileName = fileName;
	if (!f. open (QIODevice::ReadOnly)) 
	   return false;

	xmlBOM. setContent (&f);
	f. close ();
	QDomElement root	= xmlBOM. documentElement ();
	QDomElement component	= root. firstChild (). toElement ();
	while (!component. isNull ()) {
	   if (component. tagName () == "Band") {
	      bandElement bd;
	      bd. low		= component. attribute ("Low", "0"). toInt ();
	      bd. high		= component. attribute ("High", "0"). toInt ();
	      bd. label		= component. attribute ("Label", "");
	      labelTable. push_back (bd);
	   }
	   component = component. nextSibling (). toElement ();
	}
	return true;
}
