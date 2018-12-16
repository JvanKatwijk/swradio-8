#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
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
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	PMSDR_USB	
 * 	The usb interface handler for the pmsdr.
 *
 * 	This is a rewrite of parts of the code of:
 *	PMSDR command line manager (experimental)
 *
 *	Control a PMSDR hardware on Linux
 *	Modified to comply to firmware version 2.1.x
 *	Copyright (C) 2008,2009,2010  Andrea Montefusco IW0HDV, Martin Pernter IW3AUT

 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	__PMSDR_USB__
#define __PMSDR_USB__

#include	<stdlib.h>
#include	<string.h>
#include	<stdint.h>
#include	<QObject>
//#ifdef __MINGW32__
//#include	<libusb.h>
//#else
#include	<libusb-1.0/libusb.h>
//#endif

class	RadioInterface;

class	pmsdr_usb: public QObject {
Q_OBJECT
public:
			pmsdr_usb	(void);
			~pmsdr_usb	(void);
	bool		statusDevice	(void);
	
	bool		doTransaction	(uint8_t *, int16_t,
	                                 uint8_t *, int16_t *);
	int		get_transactions (int *, int *, int *);
	const char	*getError	(void);
	bool		open		(void);

private:
	bool		releaseDevice	(struct libusb_device_handle *);
	void		close		(struct libusb_device_handle *);

	uint8_t		*pszManufacturer;
	uint8_t		*pszProduct;
	bool		deviceOK;
	int16_t		usbError;

	struct libusb_device_handle	*devh;
signals:
	void		DeviceError	(int);
};

#endif

