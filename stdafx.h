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
#ifndef _stdafx_h_
#define _stdafx_h_
#pragma once
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500	//needs IDC_HAND
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501			//Need This for WS_EX_LAYERED
#endif						

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// need this for SHGetFolderPath
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>			//CCriticalSection (afx MultiThreading)
#include <math.h>
#include <shlwapi.h>
#include <afxinet.h>
#include <afxole.h>			//DragDrop Files

#include <gdiplus.h>//Used in Paste Picture (Save as jpg)
#pragma comment(lib, "gdiplus.lib")

#include <string>
#include <vector>
#include <set>
#include <map>

namespace std
{
	typedef basic_string< TCHAR > tstring;
	typedef vector<tstring> tstringvector;
};

#ifndef HDF_SORTUP
#define HDF_SORTUP 0x0400
#define HDF_SORTDOWN 0x0200
#endif

#define countof(x)	(sizeof(x) / sizeof(x[0]))

#ifndef _RELEASE
#define ALOG_ACTIVE
#endif
#include "ALog/ALog.h"

#endif

#ifndef _RELEASE
#include "UnitTest.h"
#endif

//#include "ExceptionHandler.h"

//=== For CTextFileBase
#define PEK_TX_TECHLEVEL 1

