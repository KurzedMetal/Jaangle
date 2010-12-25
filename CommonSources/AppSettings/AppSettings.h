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
#ifndef _AppSettings_h_
#define _AppSettings_h_

#include <string>

class AppSettings
{
public:
	AppSettings()				{}
	virtual ~AppSettings()		{}

	virtual BOOL Load(LPCTSTR connectionString = NULL) = 0;
	virtual BOOL Save() = 0;

	//virtual BOOL Read(LPCTSTR str, INT &value, INT defaultVal) const = 0;
	virtual BOOL Read(LPCTSTR category, LPCTSTR setting, INT &value, INT defaultVal) = 0;
	virtual BOOL Read(LPCTSTR category, LPCTSTR setting, std::basic_string<TCHAR>& value, LPCTSTR defaultVal) = 0;

	//virtual BOOL Write(const TCHAR *str, INT value) = 0;
	//virtual BOOL write(const char *str, DOUBLE value) = 0;
	//virtual BOOL Write(const char *str, LPCTSTR value) = 0;
	virtual BOOL Write(LPCTSTR category, LPCTSTR setting, LPCTSTR value) = 0;
	virtual BOOL Write(LPCTSTR category, LPCTSTR setting, INT value) = 0;
};
#endif

