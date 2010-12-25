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


class URLEncDecHelper
{
public:
	//Encodes the data with optional compression in one step
	//The PHP CODE TO DECODE this data is:
	//Case 1. bURLMode = TRUE, bEnableCompression = TRUE
	//			gzuncompress(base64_decode(strtr($_REQUEST["data"], '-_', '+/')));
	//Case 2. bURLMode = TRUE, bEnableCompression = FALSE
	//			base64_decode(strtr($_REQUEST["data"], '-_', '+/'));
	//NOT URL MODE will possible make errors if the return string is passed to a URL (not always) and the uncompression
	//			will fail
	static LPSTR EncodeData(const LPBYTE data, INT& dataLen, BOOL bURLMode, BOOL bEnableCompression);
	static LPBYTE DecodeData(LPCSTR data, INT dataLen, BOOL bURLMode, BOOL bEnableCompression);

	static LPSTR EncodeBase64(const LPBYTE data, INT& dataLen, BOOL bURLMode);
	static LPBYTE DecodeBase64(LPCSTR data, INT& dataLen, BOOL bURLMode);

#ifdef HAVE_ZLIB
	static LPBYTE CompressData(const LPBYTE data, INT& dataLen);
	static LPBYTE DeCompressData(const LPBYTE data, INT& dataLen);
#endif
	static LPSTR EncodeHex(const BYTE* byteBuffer, INT byteBufferLen);
	static BYTE* DecodeHex(LPCSTR hex, INT hexLen = -1);

};
