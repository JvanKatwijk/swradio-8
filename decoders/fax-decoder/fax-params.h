#
//	This file is to be included to get/set the parameters
//

#ifndef	__FAX_PARAMS_H
#define	__FAX_PARAMS_H

enum	FaxModes {
	Wefax576,
	Wefax288,
	HamColor,
	Color240,
	FAX480
};

typedef struct fax_params {
	const	char *Name;
	int16_t	IOC;
	int16_t	aptStart;
	int16_t	aptStop;
	bool	color;
	int16_t	lpm;
	int	nrLines;
} faxParams;


#endif

