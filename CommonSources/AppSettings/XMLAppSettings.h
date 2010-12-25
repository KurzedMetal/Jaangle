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
#ifndef _XMLAppSettings_h_
#define _XMLAppSettings_h_

#include "AppSettings.h"
//#include "CMarkup/Markup.h"

class XMLAppSettings: public AppSettings
{
public:
	XMLAppSettings();
	XMLAppSettings(LPCTSTR connectionString);
	virtual ~XMLAppSettings();

	virtual BOOL Load(LPCTSTR connectionString = NULL);
	virtual BOOL Save();

	virtual BOOL Read(LPCTSTR category, LPCTSTR setting, INT &value, INT defaultVal);
	virtual BOOL Read(LPCTSTR category, LPCTSTR setting, std::tstring& value, LPCTSTR defaultVal);

	virtual BOOL Write(LPCTSTR category, LPCTSTR setting, LPCTSTR value);
	virtual BOOL Write(LPCTSTR category, LPCTSTR setting, INT value);

private:
	std::map<CString, std::map<CString, CString> > m_settings;
	TCHAR m_Path[MAX_PATH];
	void GetDefaultFileName(LPTSTR DefaultFileName, UINT BufferLen);
};
#endif
