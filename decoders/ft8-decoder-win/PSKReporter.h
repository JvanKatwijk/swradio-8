#pragma once

// Main header file for the external interface to the PSK Reporter API
// For documentation see http://psk.gladstonefamily.net/PSKReporterAPI.pdf

/*

Copyright (c) 2008 Philip Gladstone

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

 */


#ifdef _DLL_OPTION_PSKREPORTER_EXPORT
#define DllImportExport __declspec ( dllexport )
#else
#define DllImportExport __declspec ( dllimport )
#endif


#ifdef __cplusplus
extern "C" {
#endif 

#define REPORTER_SOURCE_MASK		0x07
#define REPORTER_SOURCE_AUTOMATIC	0x01
#define REPORTER_SOURCE_LOG			0x02
#define REPORTER_SOURCE_MANUAL		0x03
#define REPORTER_SOURCE_TENTATIVE	0x40 
#define REPORTER_SOURCE_TEST		0x80 

typedef struct {
	wchar_t 	  hostname[256];
	wchar_t 	  port[32];
	bool	  	  connected;
	unsigned int  callsigns_sent;
	unsigned int  callsigns_buffered;
	unsigned int  callsigns_discarded;
	unsigned int  last_send_time;
	unsigned int  next_send_time;
	wchar_t	      last_callsign_queued[24];
	unsigned int  bytes_sent;
	unsigned int  bytes_sent_total;
	unsigned int  packets_sent;
	unsigned int  packets_sent_total;
} REPORTER_STATISTICS;




unsigned long DllImportExport __cdecl ReporterInitialize(
	const wchar_t *hostname,
	const wchar_t *port
);

unsigned long DllImportExport __cdecl ReporterSeenCallsign(
	const wchar_t *remoteInformation,
	const wchar_t *localInformation,
	unsigned long flags
);

unsigned long DllImportExport __cdecl ReporterTickle(
);

unsigned long DllImportExport __cdecl ReporterGetInformation(
	wchar_t *buffer,
	unsigned long maxlen
);

unsigned long DllImportExport __cdecl ReporterGetStatistics(
	REPORTER_STATISTICS *buffer,
	unsigned long maxlen
);

unsigned long DllImportExport __cdecl ReporterUninitialize(
);


unsigned long DllImportExport __stdcall ReporterInitializeSTD(
	const char *hostname,
	const char *port
);

unsigned long DllImportExport __stdcall ReporterSeenCallsignSTD(
	const char *remoteInformation,
	const char *localInformation,
	unsigned long flags
);

unsigned long DllImportExport __stdcall ReporterTickleSTD(
);

unsigned long DllImportExport __stdcall ReporterGetInformationSTD(
	char *buffer,
	unsigned long maxlen
);

unsigned long DllImportExport __stdcall ReporterGetStatisticsSTD(
	REPORTER_STATISTICS *buffer,
	unsigned long maxlen
);

unsigned long DllImportExport __stdcall ReporterUninitializeSTD(
);

#ifdef __cplusplus
}
#endif 


