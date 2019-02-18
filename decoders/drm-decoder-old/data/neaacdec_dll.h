#ifndef __NEAACDEC_DLL_H
#define __NEAACDEC_DLL_H

#ifdef _WIN32
# include <windows.h>
# pragma pack(push, 8)
# ifndef NEAACDECAPI
#  ifdef _MSC_VER
#    define NEAACDECAPI
#  else
#   define NEAACDECAPI __cdecl
#  endif
# endif
#else
# include <dlfcn.h>
# ifndef NEAACDECAPI
#  define NEAACDECAPI
# endif
#endif
#define DRMCH_MONO          1
#define DRMCH_STEREO        2
#define DRMCH_SBR_MONO      3
#define DRMCH_SBR_STEREO    4
#define DRMCH_SBR_PS_STEREO 5

typedef void *NeAACDecHandle;

typedef struct NeAACDecFrameInfo
{
    unsigned long bytesconsumed;
    unsigned long samples;
    unsigned char channels;
    unsigned char error;
    unsigned long samplerate;

    /* SBR: 0: off, 1: on; upsample, 2: on; downsampled, 3: off; upsampled */
    unsigned char sbr;

    /* MPEG-4 ObjectType */
    unsigned char object_type;

    /* AAC header type; MP4 will be signalled as RAW also */
    unsigned char header_type;

    /* multichannel configuration */
    unsigned char num_front_channels;
    unsigned char num_side_channels;
    unsigned char num_back_channels;
    unsigned char num_lfe_channels;
    unsigned char channel_position[64];

    /* PS: 0: off, 1: on */
    unsigned char ps;
} NeAACDecFrameInfo;

typedef NeAACDecHandle (NEAACDECAPI NeAACDecOpen_t)(void);
typedef char (NEAACDECAPI NeAACDecInitDRM_t)(NeAACDecHandle*, unsigned long, unsigned char);
typedef void (NEAACDECAPI NeAACDecClose_t)(NeAACDecHandle);
typedef void* (NEAACDECAPI NeAACDecDecode_t)(NeAACDecHandle,NeAACDecFrameInfo*,unsigned char *,unsigned long);

#define FAAD2_VERSION "(dynamically loaded)"

#endif
