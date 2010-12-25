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
#include "StdAfx.h"
#include "GdiPlusInstance.h"
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus")


//HELP
//* PRB: Microsoft Foundation Classes DEBUG_NEW Does Not Work with GDI+
//			http://support.microsoft.com/kb/317799

ULONG_PTR GdiPlusInstance::m_gdiplusToken = 0;
INT GdiPlusInstance::m_counter = 0;

GdiPlusInstance::GdiPlusInstance()
{
	m_counter++;
	//TRACE(_T("GdiPlusInstances Count: %d\r\n"),m_counter);
	if (m_gdiplusToken == 0)
	{
		TRACE(_T("GdiPlusInstances Initializing\r\n"));
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	}
}

GdiPlusInstance::~GdiPlusInstance()
{
	m_counter--;
	//TRACE(_T("~GdiPlusInstances Count: %d\r\n"),m_counter);
	if (m_counter == 0)
	{
		TRACE(_T("GdiPlusInstance::~GdiPlusInstances Shutting Down\r\n"));
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = 0;
	}
	ASSERT(m_counter >= 0);

}
