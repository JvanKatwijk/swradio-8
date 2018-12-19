#
//	This file is to be included to get/set the parameters
//

#ifndef	__FAX_PARAMS
#define	__FAX_PARAMS

enum	FaxModes {
	Wefax576,
	Wefax288,
	HamColor,
	Color240,
	FAX480
};

typedef struct fax_params {
	const char *Name;
	int16_t	IOC;
	int16_t	aptStart;
	int16_t	aptStop;
	bool	color;
	int16_t	lpm;
} faxParams;


#endif

