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
#include "IUrlDownloader.h"

LPSTR IUrlDownloader::CreateCharCopy() const
{
	ASSERT(GetStatus() == S_Success);
	LPSTR ret = NULL;
	if (GetStatus() == S_Success)
	{
		UINT bfLen = GetBufferSize();
		if (bfLen > 0)
		{
			ret = new CHAR[bfLen + 1];
			strncpy(ret, (LPCSTR)GetBuffer(), bfLen);
			ret[bfLen] = 0;
		}
	}
	return ret;
}

BOOL IUrlDownloader::SaveToFile(LPCTSTR path) const
{
	ASSERT(GetStatus() == S_Success);
	BOOL bRet = FALSE;
	if (GetStatus() == S_Success)
	{
		HANDLE f = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (f != INVALID_HANDLE_VALUE)
		{
			DWORD bytesWritten = 0;
			if (WriteFile(f, (LPCSTR)GetBuffer(), GetBufferSize(), &bytesWritten, 0) && bytesWritten)
				bRet = TRUE;
			CloseHandle(f);
		}
	}
	return bRet;
}
