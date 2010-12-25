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
#include "YearsViewSchema.h"

const TCHAR* const YearsVSString[]  = {
	_T("first(Tracks.Year)"),
	_T("count(*)"),
};

void YearsViewSchema::GetSQL(TCHAR* sql, int len, 
							  TracksFilter& Filter,
							  YearsViewSchema::Sorting sorting,
							  BOOL SortDescending)
{
	ASSERT(sql != NULL);
	ASSERT(len > 200);
	_tcsncpy(sql, _T("SELECT ")
		_T("count(*), first(Albums.Year) ")
		_T("FROM Tracks INNER JOIN Albums ON Tracks.albID = Albums.ID "),
		len);
	size_t curLen = _tcslen(sql);

	ASSERT(curLen);
	_tcscat(sql, _T(" W"));//Keep some chars for "Where"
	curLen+=2;
	size_t newLen = curLen;
	newLen += GetSQLTracksFilter(&sql[curLen], len - curLen, Filter);

	if (newLen != curLen)
	{
		sql[curLen] = 'H';
		sql[curLen+1] = 'E';
		sql[curLen+2] = 'R';
		sql[curLen+3] = 'E';
		curLen = newLen;
	}
	else
	{
		curLen -= 2;
		sql[curLen] = 0;
	}
	_tcsncat(sql, _T(" GROUP BY albums.year"), len - curLen);
	curLen = _tcslen(sql); 

	if (sorting > SORT_Disabled && sorting < SORT_Last)
	{
		curLen = _tcslen(sql);
		_sntprintf(&sql[curLen], len - curLen, _T(" ORDER BY %s"), YearsVSString[sorting - 1]);
		if (SortDescending)
		{
			curLen = _tcslen(sql);
			_tcsncat(sql, _T(" DESC"), len - curLen);
		}
	}
}

INT YearsViewSchema::GetNumTracks() 
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(0, val); 
	return val;
}

INT YearsViewSchema::GetYear() 
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(1, val); 
	return val;
}

