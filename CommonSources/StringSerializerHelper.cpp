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
#include "StringSerializerHelper.h"


BOOL StringReaderHelper::GetNextInteger(INT& value)
{
	if (m_pos == 0)
		return FALSE;
	UINT len = 0;
	LPCTSTR posEnd = _tcschr(m_pos, L',');
	LPCTSTR nextStart = NULL;
	if (posEnd != NULL)
	{
		len = posEnd - m_pos;
		nextStart = posEnd + 1;
	}
	else
	{
		len = _tcslen(m_pos);
		nextStart = &m_pos[len];
	}
	if (len > 0 && len < 20)
	{
		TCHAR bf[20];
		_tcsncpy(bf, m_pos, len);
		bf[len] = 0;
		m_pos = nextStart;
		value = _ttoi(bf);
		return TRUE;
	}
	return FALSE;
}


BOOL StringReaderHelper::GetNextBOOL(BOOL& value)
{
	INT val = 0;
	if (GetNextInteger(val) == FALSE)
		return FALSE;
	value = (val != 0);
	return TRUE;
}



BOOL StringReaderHelper::GetNextInteger(INT& value, INT minValue, INT maxValue)
{
	if (GetNextInteger(value))
		return value >= minValue && value <= maxValue;
	return FALSE;
}

BOOL StringWriterHelper::AppendBOOL(BOOL bValue)
{
	return AppendInteger(bValue ? 1 : 0);
}



BOOL StringWriterHelper::AppendInteger(INT value)
{
	INT curLen = _tcslen(m_writeString);
	INT remainBuffer = m_bfLen - curLen;
	INT len = 0;
	if (curLen > 0)
 		len = _sntprintf(&m_writeString[curLen], remainBuffer, _T(",%d"), value);
	else
		len = _sntprintf(&m_writeString[curLen], remainBuffer, _T("%d"), value);
	return len < remainBuffer;
}
