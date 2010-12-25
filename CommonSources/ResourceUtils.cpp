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
#include "ResourceUtils.h"

LPSTR GetResourceString(INT resID, LPCTSTR resType)
{
	HRSRC hres = FindResource(NULL, MAKEINTRESOURCE(resID), resType);
	if (hres != 0)
	{ 
		HGLOBAL hbytes = LoadResource(NULL, hres);
		LPVOID pdata = LockResource(hbytes);
		DWORD resSize = SizeofResource(NULL, hres);
		CHAR* str = new CHAR[resSize + 1];
		memcpy(str, pdata, resSize);
		str[resSize] = 0;
		FreeResource(hres);
		return str;
	}
	return NULL;

}

BOOL SaveResourceToFile(INT resID, LPCTSTR resType, LPCTSTR filePath)
{
	LPSTR str = GetResourceString(resID, resType);
	if (str == NULL)
		return FALSE;
	size_t strSize = strlen(str);
	HANDLE f = CreateFile(filePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	BOOL bRet = FALSE;
	if (f != NULL)
	{
		DWORD bw;
		if (WriteFile(f, str, strSize, &bw, 0) && bw==strSize)
			bRet = TRUE;
		CloseHandle(f);
	}
	return bRet;
}
