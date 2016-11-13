#pragma once

#include <stdio.h>
#include <basetsd.h>

//JPEG Headers

extern "C"
{
#include <libJPEG/jpeglib.h>
}
#ifdef _DEBUG
#pragma comment(lib, "libjpeg_d.lib" )
#else
#pragma comment(lib, "libjpeg.lib" )
#endif

#include "ShareStruct.h"

struct BLP_HEADER
{
	DWORD MagicNumber;
	DWORD Compression;
	DWORD Flags;
	DWORD Width;
	DWORD Height;
	DWORD PictureType;
	DWORD PictureSubType;
	DWORD Offset[16];
	DWORD Size[16];
};

struct BLP_RGBA
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	unsigned char Alpha;
};

struct BLP_PIXEL
{
	unsigned char Index;
};

struct my_source_manager
{
	jpeg_source_mgr man;
	unsigned char* sourceBuffer;
	long sourceBufferSize;
	JOCTET* buffer;
};

bool LoadBlpImage(IMAGEINFO *ImageInfo,void*filename,int realFileName);