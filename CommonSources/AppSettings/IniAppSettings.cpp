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
#include "IniAppSettings.h"

#define SETTING_MAX_STRING 1000

IniAppSettings::IniAppSettings()
{
	m_Path[0] = 0;
}

IniAppSettings::IniAppSettings(LPCTSTR connectionString)
{
	Load(connectionString);
}

IniAppSettings::~IniAppSettings()
{
}

BOOL IniAppSettings::Load(LPCTSTR connectionString)
{
	if (connectionString != NULL)
		_tcsncpy(m_Path, connectionString, MAX_PATH);
	if (m_Path[0] == 0)
		GetDefaultFileName(m_Path, MAX_PATH);
	return _taccess(m_Path, 6) == 0;
}

BOOL IniAppSettings::Save()
{
	return TRUE;
}

BOOL IniAppSettings::Read(LPCTSTR category, LPCTSTR setting, INT &value, INT defaultVal)
{
	ASSERT(category != NULL && setting != NULL);
	if (!(category != NULL && setting != NULL))
	{
		TRACE(_T("@1 IniAppSettings::Read category &&/|| setting empty \r\n"));
		return FALSE;
	}	
	std::basic_string<TCHAR> val;
	value = defaultVal;
	if (Read(category, setting, val, _T("")))
	{
		if (val.size() > 0)
			value = _ttoi(val.c_str());
		return TRUE; 
	}
	return FALSE;
}

BOOL IniAppSettings::Read(LPCTSTR category, LPCTSTR setting, std::basic_string<TCHAR> &value, LPCTSTR defaultVal)
{
	ASSERT(category != NULL && setting != NULL);
	if (!(category != NULL && setting != NULL))
	{
		TRACE(_T("@1 IniAppSettings::Read category &&/|| setting empty \r\n"));
		return FALSE;
	}
	ASSERT(category != NULL);
	ASSERT(setting!=NULL);
	TCHAR bf[SETTING_MAX_STRING];
	value = defaultVal == NULL ? _T("") : defaultVal;
	DWORD retChars = GetPrivateProfileString(category, setting, defaultVal, bf, SETTING_MAX_STRING, m_Path);
	if (retChars == 0)
		return FALSE;
	bf[SETTING_MAX_STRING - 1] = 0;
	ASSERT(retChars < SETTING_MAX_STRING);
	value = bf;
	return TRUE;
}

BOOL IniAppSettings::Write(LPCTSTR category, LPCTSTR setting, INT value)
{
	ASSERT(category != NULL && setting != NULL);
	if (!(category != NULL && setting != NULL))
	{
		TRACE(_T("@1 IniAppSettings::Write category &&/|| setting empty \r\n"));
		return FALSE;
	}
	TCHAR bf[30];
	_itot(value, bf, 10);
	return Write(category, setting, bf);
}

BOOL IniAppSettings::Write(LPCTSTR category, LPCTSTR setting, LPCTSTR value)
{
	ASSERT(category != NULL && setting != NULL);
	if (!(category != NULL && setting != NULL))
	{
		TRACE(_T("@1 IniAppSettings::Write category &&/|| setting empty \r\n"));
		return FALSE;
	}
	return WritePrivateProfileString(category, setting, value, m_Path);
}

void IniAppSettings::GetDefaultFileName(LPTSTR DefaultFileName, UINT BufferLen)
{
	ASSERT(DefaultFileName != NULL);
	ASSERT(BufferLen >= MAX_PATH);
	VERIFY(::GetModuleFileName(AfxGetInstanceHandle(), DefaultFileName, BufferLen));
	TCHAR *ch = _tcsrchr(DefaultFileName,'.');
	ch++;
	*ch = 0;
	_tcscat(DefaultFileName, _T("ini"));
}


