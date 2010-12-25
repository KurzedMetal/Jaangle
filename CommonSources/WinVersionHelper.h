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
#ifndef _WinVersionHelper_h_
#define _WinVersionHelper_h_

enum OsCodeEnum
{
	VST_Unknown,
	VST_Win95,
	VST_Win95OSR2,
	VST_Win98,
	VST_Win98SE,
	VST_WinME,
	VST_WinNT,
	VST_Win2000,
	VST_WinXP,
	VST_Win2003,
	VST_Vista,
	VST_Last
};

struct WindowsVersion
{
	WindowsVersion()
	{
		memset(this, 0, sizeof(WindowsVersion));
	}
	OsCodeEnum osCode;
	INT osSubCode;
	INT major;
	INT minor;
	INT pack;
	INT build;
};

INT GetWindowsVersion(WindowsVersion& verInfo);

void FormatWindowsVersion(WindowsVersion& verInfo, LPTSTR bf, UINT bfLen);

#endif
