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
#include "MSNNotifier.h"

MSNNotifier::MSNNotifier()
{

}
MSNNotifier::~MSNNotifier()
{
}

void MSNNotifier::ShowWhatImListening(LPCTSTR artist, LPCTSTR track)
{
	WCHAR buffer[1000];
	wsprintfW(buffer, L"\\0Music\\0%d\\0%s\\0%s\\0%s\\0%s\\0%s\\0", 1, L"{0} - {1}", 
		track == NULL ? L"" : (LPCWSTR)CT2CW(track), 
		artist == NULL ? L"" : (LPCWSTR)CT2CW(artist), L"", L"");
	DispatchMessage(buffer);
}

void MSNNotifier::Clear()
{
	DispatchMessage(L"\\0Music\\00\\0{0} - {1}\\0\\0\\0\\0\\0");
}

void MSNNotifier::DispatchMessage(LPWSTR msg)
{
	COPYDATASTRUCT msndata;
	msndata.dwData = 0x547;
	msndata.lpData = (void*)msg;
	msndata.cbData = (lstrlenW(msg)*2)+2;
	HWND msnui = NULL;
	while (msnui = FindWindowEx(NULL, msnui, _T("MsnMsgrUIManager"), NULL))
	{
		SendMessage(msnui, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&msndata);
	}

}
