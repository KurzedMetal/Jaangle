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
#include "InfoViewSchema.h"

void InfoViewSchema::GetSQL(TCHAR* sql, int len, 
				   UINT InfoID)
{
	_sntprintf(sql, len, _T("SELECT ID, Name, Type, DateAdded, Type FROM ")
		_T("Info WHERE ID = %d"), InfoID);

}
void InfoViewSchema::GetSQLJoined(TCHAR* sql, int len, 
								  ITEM_INFO_TYPE iit, UINT itemID)
{
	const TCHAR* tblName = _T("Tracks");
	const TCHAR* fldName = _T("InfoID");
	switch (iit)
	{
	case IIT_ArtistBio:
		tblName = _T("Artists");//Keeping the default fieldName
		break;
	case IIT_AlbumReview:
		tblName = _T("Albums");//Keeping the default fieldName
		break;
	case IIT_TrackComment:
		fldName = _T("comID");//Keeping the default Table Name
		break;
	case IIT_TrackLyrics:
		fldName = _T("lyrID");//Keeping the default Table Name
		break;
	case IIT_TrackPersonal:
		fldName = _T("perID");//Keeping the default Table Name
		break;
	default:
		ASSERT(0);//???
		break;
	}
	_sntprintf(sql, len, _T("SELECT Info.ID, Info.Name, Info.Type, Info.DateAdded, Info.Type ")
		_T("FROM Info INNER JOIN %s ON Info.ID = %s.%s WHERE %s.ID = %d"),
		tblName, tblName, fldName, tblName, itemID);

}
//Artists Main Table
INT InfoViewSchema::GetInfoID()
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(0, val); 
	return val;

}

void InfoViewSchema::GetInfo(CString& info)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(1, info);
}

INT InfoViewSchema::GetType()
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(2, val); 
	return val;

}
void InfoViewSchema::GetDateAdded(SYSTEMTIME& t)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(3, t);

}
