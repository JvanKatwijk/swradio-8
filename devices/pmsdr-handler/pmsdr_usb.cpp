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
 * 	This is a  partial rewrite of the code of:
 *	PMSDR command line manager (experimental)
 *	together with some code from the rtl2832u interface
 *	from osmocom
 *
 *	Control a PMSDR hardware on Linux
 *	Modified to comply to firmware version 2.1.x
 *	Copyright (C) 2008,2009,2010  Andrea Montefusco IW0HDV,
 *	Martin Pernter IW3AUT
 */
#include	"pmsdr_usb.h"
#include	<stdint.h>
#include	<stdio.h>

enum pmsdr_errcode {
	PMSDR_NO_ERROR               = 0,
	PMSDR_NO_SUCH_DEVICE         = 1,
	PMSDR_INVALID_CTX            = 2,
	PMSDR_FOUND_UNABLE_TO_OPEN   = 3,
	PMSDR_UNABLE_TO_GET_USB_LIST = 4,
	PMSDR_CANT_OPEN_DEVICE       = 5,
	PMSDR_DEVICE_NOT_READY       = 6,
	PMSDR_GENERIC_ERROR          = -32678,
};

typedef struct pmsdr_device {
	uint16_t	vid;
	uint16_t	pid;
	const	char	*name;
} pmsdr_device_t;


#define PMSDR_VENDOR_ID    0x04d8 
#define PMSDR_PRODUCT_ID   0x000c

#define SR_VENDOR_ID       0x16c0 
#define SR_PRODUCT_ID      0x05dc

static	pmsdr_device_t devices [] = {
	{SR_VENDOR_ID, SR_PRODUCT_ID, "pmsdr"},
	{PMSDR_VENDOR_ID, PMSDR_PRODUCT_ID, "pmsdr"}
};

static
pmsdr_device_t	*findDevice (uint16_t vid, uint16_t pid) {
uint16_t		i;

	for (i = 0; i < sizeof (devices) /sizeof (pmsdr_device_t); i ++) {
	   if (devices [i]. vid == vid &&
	       devices [i]. pid == pid) 
	      return &devices [i];
	}

	return NULL;
}
	
#define	pmsdr_BUFSIZE	256
//
//	usb device handler for pmsdr
//
	pmsdr_usb::pmsdr_usb   (void) {
libusb_device **ulist;
int16_t	i = 0;
int16_t	nud;
libusb_device *pDev;
struct libusb_device_descriptor Desc;

	deviceOK	= false;
	devh		= NULL;
	usbError	= PMSDR_NO_ERROR;
	if (libusb_init (NULL) < 0) {
           fprintf(stderr, "Failed to initialise libusb\n");
	   usbError	= PMSDR_GENERIC_ERROR;
	   return;
        }

	libusb_set_debug (NULL, 3);
	nud = libusb_get_device_list (NULL, &ulist); 
	if  (nud <= 0) {
	   fprintf (stderr, "Unable to get a list of USB devices.\n");
	   usbError	= PMSDR_UNABLE_TO_GET_USB_LIST;
	   return;
	}

	for (i = 0; i < nud; ++i) {
	   pDev = ulist[i];
	   fprintf (stderr, "Bus %03d Device %03d ",
	                     libusb_get_bus_number (pDev),
	                     libusb_get_device_address (pDev) );

	   if (libusb_get_device_descriptor (pDev, &Desc) == 0) {
	      printf("%d: %04x:%04x (bus %d, device %d)", i,
	               Desc.idVendor, Desc.idProduct,
	               libusb_get_bus_number (pDev),
	               libusb_get_device_address (pDev));

	      if (findDevice (Desc. idVendor, Desc. idProduct) != NULL)
	         printf(" ***\n");
	      else
	         printf("\n");
	   }
	}

	libusb_free_device_list (ulist, 0);
	deviceOK	= true;
}

	pmsdr_usb::~pmsdr_usb (void) {
//	fprintf (stderr, "closing usb part of pmsdr, with device = %d\n",
//	                                                 deviceOK);
	if (!deviceOK)
	   return;
	close (devh);
//	disconnect (this, SIGNAL (DeviceError (int)),
//	            mainInterface, SLOT (HandleDeviceError (int)));
        libusb_exit (NULL);
}

bool	pmsdr_usb::open (void) {
libusb_device **list;
libusb_device	*device	= NULL;
struct libusb_device_descriptor dd;
struct libusb_device_descriptor devDesc;
int32_t		deviceCount	= 0;
int32_t		r, i;
uint16_t	cnt;
int16_t		index	= 0;
uint8_t		szManufacturer	[pmsdr_BUFSIZE];
uint8_t		szProduct	[pmsdr_BUFSIZE];

	usbError	= PMSDR_NO_ERROR;

	cnt	= libusb_get_device_list (NULL, &list);
	for (i = 0; i < cnt; i ++) {
	   libusb_get_device_descriptor (list [i], &dd);
	   if (findDevice (dd. idVendor, dd.idProduct))
	      index ++;
	}

	if (index == 0) {
	   fprintf (stderr, "No supported devices found\n");
	   goto err;
	}

	index	= 0;
//	now find one for the open
	for (i = 0; i < cnt; i ++) {
	   device = list [i];
	   libusb_get_device_descriptor (list [i], &dd);
	   if (findDevice (dd. idVendor, dd.idProduct) != NULL) {
	      devDesc = dd;
	      deviceCount ++;
	   }

	   if (index == (int)deviceCount - 1)
	      break;

	   device = NULL;
	}

	if (device == NULL) {
	   r = -1;
	   goto err;
	}
//
//	apparently, we do have a device,
//	get the ascii data and show it
	r = libusb_open (device, &devh);
	if (r < 0) {
	   libusb_free_device_list (list, 0);
	   fprintf (stderr, "usb open error %d\n", r);
	   goto err;
	}

	if (libusb_get_string_descriptor_ascii (devh,
	                                        devDesc.iManufacturer,
	                                        szManufacturer,
	                                        sizeof szManufacturer) >= 0 &&
	    libusb_get_string_descriptor_ascii (devh,
	                                        devDesc.iProduct,
	                                        szProduct,
	                                        sizeof szProduct) >= 0) 
	     fprintf (stderr, "Found %s %s\n", szManufacturer, szProduct);

	libusb_free_device_list (list, 0);

	if (libusb_reset_device (devh) != 0) {
	   fprintf (stderr, "Could not reset device with ID %04X:%04X\n",
	                  dd. idVendor, dd. idProduct);
	   goto err;
	}

	if (!releaseDevice (devh))
	   return false;

	r = libusb_claim_interface (devh, 0);
	if (r < 0) {
	   fprintf (stderr, "usb: claim-interface error %d\n", r);
	}

	return true;

err:
	libusb_exit (NULL);
	return false;
}

void	pmsdr_usb::close (struct libusb_device_handle *devh) {
        if (devh) {
            libusb_release_interface (devh, 0);
            libusb_close (devh);
        }
}

bool	pmsdr_usb::doTransaction (uint8_t *pTxData, int16_t txLen,
                                  uint8_t *pRxData, int16_t *pRxLen) {
int	transferred = 0;
int16_t	rc;
int	tmp;

	usbError	= PMSDR_NO_ERROR;
	rc = libusb_interrupt_transfer (devh,
	                                0x01,
	                                pTxData,
    		                        txLen,
	                                &transferred,
	                                5000);
	
	if (rc < 0) {
	   if (rc == LIBUSB_ERROR_NO_DEVICE)
	      emit DeviceError (rc);
	   fprintf (stderr, "USB Transfer error\n");
	   usbError	= PMSDR_GENERIC_ERROR;
	   return false;
	}
 
	if (pRxData && *pRxLen) {	// response required
	   rc = libusb_interrupt_transfer (devh,
	                                   0x81,
	                                   pRxData,
	                                   *pRxLen,
	                                   &tmp,
	                                   5000);
	   *pRxLen = tmp;
	   if (rc < 0) {
	      if (rc == LIBUSB_ERROR_NO_DEVICE)
	         emit DeviceError (rc);
	      fprintf(stderr, "usb_interrupt_transfer error %d\n", rc );
	      usbError	= rc;
	      return false;
	   }
  
	}  // else part not required, transaction doesn't require response

	return true;
}

const char	*pmsdr_usb::getError (void) {
	switch (usbError) {
	   case PMSDR_NO_ERROR : return "no_error";
	   case PMSDR_NO_SUCH_DEVICE : return "no such device";
	   case PMSDR_INVALID_CTX : return "invalid ctx";
	   case PMSDR_FOUND_UNABLE_TO_OPEN : return "found unable to open";
	   case PMSDR_UNABLE_TO_GET_USB_LIST : return "unable to get USB list";
	   case PMSDR_CANT_OPEN_DEVICE : return "Cant open device";
	   case PMSDR_DEVICE_NOT_READY : return "Device not Ready";
	   default:
	   case PMSDR_GENERIC_ERROR : return "Generic error";
	}

	return "cannot happen";
}

bool	pmsdr_usb::statusDevice (void) {
	return deviceOK;
}
//	check if the device has been got by the kernel,
//	but this feature does not work in libusb-1.0 under windows,
//	so - rather blunt - we'll skip it
//
bool	pmsdr_usb::releaseDevice (struct libusb_device_handle *devh) {
int	r;
#ifndef	__MINGW32__
	r = libusb_kernel_driver_active (devh, 0);

	fprintf (stderr, "Kernel driver enquiry: %d\n", r);
	switch (r) {
	   case 0:   // no kernel driver is active, do nothing
	   break;

	   case 1:   // a kernel driver is active
	      fprintf (stderr,
	         "Warning: Kernel device driver already active. Trying to detach....");
	      if ( (r = libusb_detach_kernel_driver (devh, 0)) ) {
	         fprintf (stderr, "detach FAILED: %d.\n", r);
                } else {
                    fprintf (stderr, "SUCCESS.\n");
                }
	      break;
//
	   case LIBUSB_ERROR_NO_DEVICE:   // the device has been disconnected
	      fprintf (stderr, "Device disconnected.\n");
	      usbError	= PMSDR_GENERIC_ERROR;
	      return false;

	   default:
	      fprintf (stderr, "Unexpected error: %d\n", r);
	      usbError	= PMSDR_GENERIC_ERROR;
	      return false;
	}
#endif
	return true;
}

