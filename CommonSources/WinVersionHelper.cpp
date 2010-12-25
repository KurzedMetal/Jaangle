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
#include "WinVersionHelper.h"

BOOL GetWindowsVersion(WindowsVersion& verInfo)
{
	OSVERSIONINFOEX osvi;
	TCHAR szPack[4] = _T("-1");


	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi,sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bResult = GetVersionEx((OSVERSIONINFO*)&osvi);
	if(!bResult)
	{
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	// The system Type (2=Win95, 3=Win98/ME, 
	//    4=Win/ME, 5=Win/NT  6=Win/2000)

	//if(osvi.dwPlatformId == VER_PLATFORM_WIN32s)
	//	*iSysType=VST_UnknownTemp;
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		verInfo.osCode = VST_Win95;
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		verInfo.osCode = VST_WinNT;

	// Major version number
	verInfo.major=osvi.dwMajorVersion;
	if(verInfo.major == 5) verInfo.osCode=VST_Win2000;
	if(verInfo.major == 6) verInfo.osCode=VST_Vista;

	// Minor version number
	verInfo.minor=osvi.dwMinorVersion;

	// For Win/98, minor version is 10
	if((verInfo.osCode==VST_Win95) && (verInfo.minor==10)) verInfo.osCode=VST_Win98;
	// For Win/ME, minor version is 90 or greater
	if((verInfo.osCode==VST_Win95) && (verInfo.minor==90)) verInfo.osCode=VST_WinME;
	// For Win/XP, minor version is 1
	if((verInfo.osCode==VST_Win2000) && (verInfo.minor==1)) verInfo.osCode=VST_WinXP;
	// For Win/2003, minor version is 2
	if((verInfo.osCode==VST_Win2000) && (verInfo.minor==2)) verInfo.osCode=VST_Win2003;

	// Service Pack info
	// For NT/2000/later, extract the service pack as a number
	verInfo.pack=0;
	if((verInfo.osCode>=VST_WinNT) && (_tcscmp(osvi.szCSDVersion,_T("Service Pack"))))
	{
		szPack[0]=osvi.szCSDVersion[12];
		szPack[1]=osvi.szCSDVersion[13];
		szPack[2]=osvi.szCSDVersion[14];
		verInfo.pack=_ttoi(szPack);
	}

	// For Win/95, extract OSR info and return in iSubSys
	if(verInfo.osCode==VST_Win95)
	{
		if(osvi.szCSDVersion[1]=='C' || osvi.szCSDVersion[1]=='B' )
			verInfo.osCode=VST_Win95OSR2;
	}

	// For Win/98, extract OSR info and return in iSubSys
	if(verInfo.osCode==VST_Win98)
	{
		if(osvi.szCSDVersion[1]=='A')
			verInfo.osCode=VST_Win98SE;                // Second Edition
	}

	// For NT/2000/XP/later, determine whether Home, Professional or Server
	// or Advanced Server

	if(verInfo.osCode>=VST_WinNT)
	{
		if(bResult)
		{
			// See if Workstation or Server
			if(osvi.wProductType == VER_NT_WORKSTATION)
			{
				// See if personal edition
				if(osvi.wSuiteMask & VER_SUITE_PERSONAL)
				{
					verInfo.osSubCode=1;
				}
				else
				{
					verInfo.osSubCode=2;  // Must be Professional
				}
			}

			if(osvi.wProductType == VER_NT_SERVER ||
				osvi.wProductType == VER_NT_DOMAIN_CONTROLLER)
			{
				// See if data center edition
				if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
				{
					verInfo.osSubCode=5;  // Datacenter Server
				}
				else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					verInfo.osSubCode=4;  // Advanced Server
				}
				else if(osvi.wSuiteMask & VER_SUITE_BACKOFFICE)
				{
					verInfo.osSubCode=6;  // BackOffice Server
				}
				else
				{
					verInfo.osSubCode=3;  // Must be Server
				}
			}
		}
		else
		{
			// Unable to retrieve OSVERSIONINFOEX, get from 
			// registry (for older versions of NT) instead
			HKEY hKey;
			TCHAR szProductType[80];
			DWORD dwBufLen;

			RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				_T("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
				0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueEx( hKey, _T("ProductType"), NULL, NULL,
				(LPBYTE)szProductType, &dwBufLen);

			RegCloseKey( hKey );
			if(lstrcmpi( _T("WINNT"), szProductType) == 0 )
				verInfo.osSubCode=2;
			if(lstrcmpi( _T("LANMANNT"), szProductType) == 0 )
				verInfo.osSubCode=3;
			if(lstrcmpi( _T("SERVERNT"), szProductType) == 0 )
				verInfo.osSubCode=4;
		}

		// Build number
		verInfo.build=osvi.dwBuildNumber & 0xFFFF;
	}

	return TRUE;
}

void FormatWindowsVersion(WindowsVersion& verInfo, LPTSTR bf, UINT bfLen)
{
	LPCTSTR osName = _T("Unknown");
	switch (verInfo.osCode)
	{
	case VST_Win95:		osName=_T("Win95");break;
	case VST_Win95OSR2:	osName=_T("Win95OSR2");break;
	case VST_Win98:		osName=_T("Win98");break;
	case VST_Win98SE:	osName=_T("Win98SE");break;
	case VST_WinME:		osName=_T("WinME");break;
	case VST_WinNT:		osName=_T("WinNT");break;
	case VST_Win2000:	osName=_T("Win2000");break;
	case VST_WinXP:		osName=_T("WinXP");break;
	case VST_Win2003:	osName=_T("Win2003");break;
	case VST_Vista:		osName=_T("Vista");break;
	}

	TCHAR sp[20];
	if (verInfo.pack > 0)
		_sntprintf(sp, 20, _T(" SP%d"), verInfo.pack);
	else
		sp[0] = 0;
	_sntprintf(bf, bfLen, _T("%s (%d.%d.%d)%s"), 
		osName, 
		verInfo.major, verInfo.minor, verInfo.build, 
		sp);
	bf[bfLen - 1] = 0;
}

