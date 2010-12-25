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
#pragma once

#define MAX_VALUE_HEADER 30

class StringReaderHelper
{
public:
	StringReaderHelper():m_loadString(NULL)	{}

	void SetReadString(LPCTSTR loadString)		
	{
		m_loadString = loadString; 
		m_pos = m_loadString;
	}
	BOOL GetNextBOOL(BOOL& value);
	BOOL GetNextInteger(INT& value);
	BOOL GetNextInteger(INT& value, INT minValue, INT maxValue);

private:
	LPCTSTR m_loadString;
	LPCTSTR m_pos;
	TCHAR m_header[MAX_VALUE_HEADER];
};

class StringWriterHelper
{
public:
	StringWriterHelper():m_writeString(NULL),m_bfLen(0)		{}

	void SetWriteString(LPTSTR writeString, UINT bfLen)	
	{
		m_writeString = writeString;
		m_bfLen = bfLen;
		if (m_writeString != NULL && m_bfLen > 0)
			m_writeString[0] = 0;
	}
	BOOL AppendBOOL(BOOL bValue);
	BOOL AppendInteger(INT value);
private:
	LPTSTR m_writeString;
	UINT m_bfLen;

};
