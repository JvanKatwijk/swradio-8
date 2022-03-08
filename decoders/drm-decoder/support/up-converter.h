#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM+ Decoder
 *
 *    DRM+ Decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM+ Decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM+ Decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__UP_CONVERTER_H
#define	__UP_CONVERTER_H

#include	<math.h>
#include	<complex>
#include	<vector>
#include	<stdint.h>
#include	<unistd.h>
#include	"fir-filters.h"

class	converter_base;

class	audioConverter {
private:
	int32_t		inRate;
	int32_t		outRate;
	converter_base	*workerHandle;
public:
		audioConverter (int32_t inRate, int32_t outRate, 
	                      int32_t inSize);

		~audioConverter ();

bool	convert (std::complex<float> v,
	                       std::complex<float> *out, int32_t *amount);

int32_t	getOutputSize ();
};


class	converter_base {
public:
	converter_base	();
virtual	~converter_base	();

virtual
bool	convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
virtual
int	getOutputSize	();
protected:
	std::vector<std::complex<float>> buffer;
	int inP;
};

class	converter_9600 : public converter_base {
public:
	converter_9600		();
	~converter_9600		();
bool	convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};


class	converter_12000 : public converter_base {
public:
	converter_12000		();
	~converter_12000	();
bool	convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};

class	converter_16000 : public converter_base {
public:
	converter_16000		();
	~converter_16000	();
bool	convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};

class	converter_19200 : public converter_base {
public:
	converter_19200		();
	~converter_19200	();
bool	convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};

class	converter_24000 : public converter_base {
public:
	converter_24000		();
	~converter_24000	();
bool	convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};

class	converter_32000 : public converter_base {
public:
	converter_32000		();
	~converter_32000	();
bool	convert		(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};

class	converter_38400 : public converter_base {
public:
	converter_38400		();
	~converter_38400	();
bool	convert			(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};

class	converter_48000 : public converter_base {
public:
	converter_48000		();
	~converter_48000	();
bool	convert			(std::complex<float> v,
	                                 std::complex<float> *out,
	                                 int *amount);
int	getOutputSize	();
private:
	LowPassFIR	theFilter;
};

#endif
