#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  SDR-J 
 *    Many of the ideas as implemented in the SDR-J are derived from
 *    other work, made available through the (a) GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"popup-keypad.h"
#include	"radio.h"
//
//	A simple keypad for the FM receiver. 13 buttons and
//	an LCD display
	keyPad::keyPad (RadioInterface *mr) {
	this	-> myRadio	= mr;
	theFrame	= new QWidget;
	theLayout	= new QGridLayout;
	thePad		= new QButtonGroup;
	oneButton	= new QPushButton ("1");
	theLayout	-> addWidget (oneButton, 1, 1);
	twoButton	= new QPushButton ("2");
	theLayout	-> addWidget (twoButton, 1, 2);
	threeButton	= new QPushButton ("3");
	theLayout	-> addWidget (threeButton, 1, 3);
	fourButton	= new QPushButton ("4");
	theLayout	-> addWidget (fourButton, 2, 1);
	fiveButton	= new QPushButton ("5");
	theLayout	-> addWidget (fiveButton, 2, 2);
	sixButton	= new QPushButton ("6");
	theLayout	-> addWidget (sixButton, 2, 3);
	sevenButton	= new QPushButton ("7");
	theLayout	-> addWidget (sevenButton, 3, 1);
	eightButton	= new QPushButton ("8");
	theLayout	-> addWidget (eightButton, 3, 2);
	nineButton	= new QPushButton ("9");
	theLayout	-> addWidget (nineButton, 3, 3);
	KHzButton	= new QPushButton ("Khz");
	theLayout	-> addWidget (KHzButton, 4, 1);
	MHzButton	= new QPushButton ("Mhz");
	theLayout	-> addWidget (MHzButton, 4, 3);
	zeroButton	= new QPushButton ("0");
	theLayout	-> addWidget (zeroButton, 4, 2);
	clearButton	= new QPushButton ("clear");
	theLayout	-> addWidget (clearButton, 5, 1);
	correctButton	= new QPushButton ("correct");
	theLayout	-> addWidget (correctButton, 5, 3);
	theDisplay	= new QLCDNumber (8);
	theDisplay	-> setMode (QLCDNumber::Dec);
	theDisplay	-> setSegmentStyle (QLCDNumber::Flat);
	theLayout	-> addWidget (theDisplay, 5, 2);
	theFrame	-> setLayout (theLayout);

	thePad		-> addButton (zeroButton, 0);
	thePad		-> addButton (oneButton, 1);
	thePad		-> addButton (twoButton, 2);
	thePad		-> addButton (threeButton, 3);
	thePad		-> addButton (fourButton, 4);
	thePad		-> addButton (fiveButton, 5);
	thePad		-> addButton (sixButton, 6);
	thePad		-> addButton (sevenButton, 7);
	thePad		-> addButton (eightButton, 8);
	thePad		-> addButton (nineButton, 9);
	thePad		-> addButton (KHzButton, 101);
	thePad		-> addButton (MHzButton, 102);
	thePad		-> addButton (clearButton, 103);
	thePad		-> addButton (correctButton, 104);
	theDisplay	-> display (0);
	connect (thePad, SIGNAL (buttonClicked (int)),
	         this, SLOT (collectData (int)));
	connect (this, SIGNAL (newFrequency (int)),
	         mr, SLOT (setFrequency (int)));
//
//	initially the keypad is not visible,
	panel		= 0;
	shown		= false;
}


	keyPad::~keyPad	(void) {
//	delete	oneButton;
//	delete	twoButton;
//	delete	threeButton;
//	delete	fourButton;
//	delete	fiveButton;
//	delete	sixButton;
//	delete	sevenButton;
//	delete	eightButton;
//	delete	nineButton;
//	delete	zeroButton;
//	delete	KHzButton;
//	delete	clearButton;
//	delete	theDisplay;
//	delete	theDisplay;
//	delete	thePad;
	delete	theFrame;
}
//
void	keyPad::showPad	(void) {
	theFrame	-> show ();
	shown		= true;
	panel		= 0;
}

void	keyPad::hidePad	(void) {
	theFrame	-> hide ();
	shown		= false;
}

bool	keyPad::isVisible	(void) {
	return shown;
}

void	keyPad::collectData	(int id) {
	if (0 <= id && id < 10) {
	   panel = 10 * panel + id;
	   theDisplay	-> display (panel);
	}
	else
	if (id == 101) {		// Khz button
	   newFrequency (Khz (panel));
	   panel = 0;
	   hidePad ();
	}
	else
	if (id == 102) {		// Mhz button
	   newFrequency (Mhz (panel));
	   panel = 0;
	   hidePad ();
	}
	else
	if (id == 103) {		// clear Button
	   panel = 0;
	   theDisplay	-> display (panel);
	}
	else
	if (id == 104) {		// correct Button
	   panel = panel / 10;
	   theDisplay	-> display (panel);
	}
}

