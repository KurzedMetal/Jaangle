//	/*
// 	*
// 	* Copyright (C) 2003-2010 Alexandros Economou
//	*
//	* This file is part of Jaangle (http://www.jaangle.com)
// 	*
// 	* This Program is free software; you can redistribute it and/or modify
// 	* it under the terms of the GNU General Public License as published by
// 	* the Free Software Foundation; either version 2, or (at your option)
// 	* any later version.
// 	*
// 	* This Program is distributed in the hope that it will be useful,
// 	* but WITHOUT ANY WARRANTY; without even the implied warranty of
// 	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// 	* GNU General Public License for more details.
// 	*
// 	* You should have received a copy of the GNU General Public License
// 	* along with GNU Make; see the file COPYING. If not, write to
// 	* the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
// 	* http://www.gnu.org/copyleft/gpl.html
// 	*
//	*/ 

#include "stdafx.h"
#include "URLEncDecHelper.h"

#ifdef HAVE_ZLIB
#ifndef ZLIB_WINAPI
#define ZLIB_WINAPI
#endif
#include "zlib/zlib.h"
#endif
#include "atlenc.h"

LPSTR URLEncDecHelper::EncodeData(const LPBYTE data, INT& dataLen, BOOL bURLMode, BOOL bEnableCompression)
{
	LPBYTE compData = NULL;
	if (bEnableCompression)
	{
#ifdef HAVE_ZLIB
		compData = CompressData(data, dataLen);
		if (compData == NULL)
			return NULL;
#else
		ASSERT(0);
#endif
	}
	LPSTR ret = EncodeBase64(bEnableCompression ? compData : data, dataLen, bURLMode);
	delete[] compData;
	return ret;
}

LPBYTE URLEncDecHelper::DecodeData(LPCSTR data, INT dataLen, BOOL bURLMode, BOOL bEnableCompression)
{
	BYTE* ret = DecodeBase64(data, dataLen, bURLMode);
	if (ret != NULL)
	{
		if (bEnableCompression)
		{
#ifdef HAVE_ZLIB
			LPBYTE compData = NULL;
			compData = DeCompressData(ret, dataLen);
			if (compData == NULL)
			{
				delete ret;
				ret = NULL;
			}
			else
			{
				delete ret;
				ret = compData;
			}
#else
			ASSERT(0);
#endif
		}	
	}
	return ret;
}


LPSTR URLEncDecHelper::EncodeBase64(LPBYTE data, INT& dataLen, BOOL bURLMode)
{
	INT base64DataLen = Base64EncodeGetRequiredLength(dataLen, ATL_BASE64_FLAG_NOCRLF) + 1;
	LPSTR base64Data = new CHAR[base64DataLen];
	if (Base64Encode(data, dataLen, base64Data, &base64DataLen, ATL_BASE64_FLAG_NOCRLF))
	{
		dataLen = base64DataLen;
		if (bURLMode)
		{
			for (INT i = 0; i <= base64DataLen; i++)
			{
				switch (base64Data[i])
				{
				case '+':
					base64Data[i] = '-';
					break;
				case '/':
					base64Data[i] = '_';
					break;
				}
			}
		}
		base64Data[base64DataLen] = 0;
		return base64Data;
	}
	delete[] base64Data;
	return NULL;
}

LPBYTE URLEncDecHelper::DecodeBase64(LPCSTR data, INT& dataLen, BOOL bURLMode)
{
	if (dataLen == -1)
		dataLen = (INT)strlen(data);
	INT plainDataLen = Base64DecodeGetRequiredLength(dataLen);
	BYTE* plainData = new BYTE[plainDataLen + 1];
	if (Base64Decode((LPCSTR)data, dataLen, plainData, &plainDataLen))
	{
		dataLen = plainDataLen;
		if (bURLMode)
		{
			for (INT i = 0; i < dataLen; i++)
			{
				switch (plainData[i])
				{
				case '-':
					plainData[i] = '+';
					break;
				case '_':
					plainData[i] = '/';
					break;
				}
			}
		}
		return plainData;
	}
	delete[] plainData;
	return NULL;
}

//=== strBufferLen should be > byteBufferLen * 2
LPSTR URLEncDecHelper::EncodeHex(const BYTE* byteBuffer, INT byteBufferLen)
{
	ASSERT(byteBuffer != NULL);
	INT hexLen = byteBufferLen * 2;
	CHAR* ret = new CHAR[hexLen + 1];
	for (INT i = 0; i < byteBufferLen; i++)
		_snprintf(&ret[i * 2], 3, "%02X", byteBuffer[i]);
	return ret;
}

//=== byteBufferLen should be > byteBufferLen / 2
LPBYTE URLEncDecHelper::DecodeHex(LPCSTR hex, INT hexLen /*= -1*/)
{
	if (hexLen == -1)
		hexLen = strlen(hex);
	ASSERT(hexLen % 2 == 0);
	if (hexLen % 2 != 0)
		return NULL;
	INT byteLen = hexLen / 2;
	BYTE* ret = new BYTE[byteLen];

	CHAR bf[3];
	bf[2] = 0;
	LPSTR dummyBf = NULL;
	for (INT i = 0; i < byteLen; i++)
	{
		memcpy(bf, &hex[i * 2], 2);
		ret[i] = (BYTE)strtol(bf, &dummyBf, 16);
	}
	return ret;
}

#ifdef HAVE_ZLIB

LPBYTE URLEncDecHelper::CompressData(const LPBYTE data, INT& dataLen)
{
	ULONG compressedDataLen = 2 * dataLen;
	BYTE* compressedData = new BYTE[compressedDataLen];
	int ret = compress(compressedData, &compressedDataLen, (BYTE*)data, dataLen);
	if (ret == Z_OK)
	{
		dataLen = compressedDataLen;
		return compressedData;
	}
	delete[] compressedData;
	return NULL;
}

LPBYTE URLEncDecHelper::DeCompressData(const LPBYTE data, INT& dataLen)
{
	ULONG uncompressedDataLen = 10 * dataLen;
	BYTE* uncompressedData = new BYTE[uncompressedDataLen];
	if (uncompress(uncompressedData, &uncompressedDataLen, (BYTE*)data, dataLen) == Z_OK)
	{
		dataLen = uncompressedDataLen;
		return uncompressedData;
	}
	delete[] uncompressedData;
	return NULL;

}

#endif
