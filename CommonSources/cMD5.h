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

#if !defined(AFX_CMD5_H__6C2F7C61_93D8_11D8_A8BA_000AE637F271__INCLUDED_)
#define AFX_CMD5_H__6C2F7C61_93D8_11D8_A8BA_000AE637F271__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _ReadBufSize 1000000

class cMD5  
{
public:
	char* CalcMD5FromString(const char *s8_Input);
	char* CalcMD5FromWString(const wchar_t *buffer);
	char* CalcMD5FromBuffer(const BYTE *buffer, UINT bfLen);
	char* CalcMD5FromFile  (LPCTSTR s8_Path);

	void FreeBuffer();
	cMD5();
	virtual ~cMD5();

private:
	struct MD5Context 
	{
		unsigned long buf[4];
		unsigned long bits[2];
		unsigned char in[64];
	};

	void MD5Init();
	void MD5Update(unsigned char *buf, unsigned len);
	void MD5Final (unsigned char digest[16]);

	void MD5Transform(unsigned long buf[4], unsigned long in[16]);
	char* MD5FinalToString();

	void byteReverse (unsigned char *buf, unsigned longs);

	char *mp_s8ReadBuffer;
	MD5Context ctx;
	char   ms8_MD5[40]; // Output buffer
};

#endif // !defined(AFX_CMD5_H__6C2F7C61_93D8_11D8_A8BA_000AE637F271__INCLUDED_)
