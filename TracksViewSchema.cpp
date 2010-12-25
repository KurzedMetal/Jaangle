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
#include "stdAfx.h"
#include "TracksViewSchema.h"
#include "TracksTableSchema.h"


const TCHAR* const TracksVSString[]  = 
{ 
	_T("[Tracks.Name]"), 
	_T("[Artists.Name]"),
	_T("[Albums.Name]"),
	_T("[Tracks.TrackNo]"),
	_T("[Tracks.Path]"),
	_T("[Collections.Name]"),
	_T("[Tracks.Size]"),
	_T("[Tracks.Duration]"),
	_T("[Tracks.Bitrate]"),
	_T("[Tracks.Year]"),
	_T("[Genres.Name]"),
	_T("[Tracks.DateAdded]"),
	_T("[Tracks.Rating]")
};


//enum Sorting
//{
//	SORT_Disabled = 0,
//	SORT_Name,
//	SORT_Artist,
//	SORT_Album, 
//	SORT_TrackNo,
//	SORT_Location,
//	SORT_Collection,
//	SORT_Size,
//	SORT_Duration,
//	SORT_BitRate,
//	SORT_Year,
//	SORT_Genre, 
//	SORT_DateAdded,
//	SORT_Rating,
//	SORT_Last
//};



//Returns SELECT ... FROM ... (JOINS)
//RetVal = Len
INT TracksViewSchema::GetSQLSelect(TCHAR* sql, int len)
{
	ASSERT(sql != NULL);
	ASSERT(len > 300);
	//_tcsncpy(sql, _T("SELECT ")
	//	_T("[Tracks.Rating],[Tracks.Name],[Albums.Name],[Artists.Name],")
	//	_T("[Tracks.TrackNo],[Tracks.Path],[Tracks.Size],[Tracks.Duration],")
	//	_T("[Tracks.Bitrate],[Tracks.Year],[Genres.Name],[Tracks.DateAdded],")
	//	_T("[Tracks.ID],[Tracks.artID],[Tracks.albID],[Tracks.Type],")
	//	_T("[Tracks.LyrID],[Tracks.perID], [Tracks.comID], [Genres.ID], [Tracks.SyncState], [Tracks.WriteableState] ")
	//	_T("FROM ((Tracks INNER JOIN Albums ON Tracks.albID=Albums.ID) ")
	//	_T("INNER JOIN Artists ON Tracks.artID = Artists.ID) ")
	//	_T("INNER JOIN Genres ON Tracks.genreID = Genres.ID"),
	//	len);
	_tcsncpy(sql, _T("SELECT ")
		_T("[Tracks.Rating],[Tracks.Name],[Albums.Name],[Artists.Name],")
		_T("[Tracks.TrackNo],[Tracks.Path],[Tracks.Size],[Tracks.Duration],")
		_T("[Tracks.Bitrate],[Tracks.Year],[Genres.Name],[Tracks.DateAdded],")
		_T("[Tracks.ID],[Tracks.artID],[Tracks.albID],[Tracks.Type],")
		_T("[Tracks.LyrID],[Tracks.perID],[Tracks.comID],[Genres.ID],")
		_T("[Tracks.SyncState],[Tracks.WriteableState],[Collections.Type],[Collections.Name],")
		_T("[Collections.ID] ")
		_T("FROM (((Tracks INNER JOIN Albums ON Tracks.albID=Albums.ID) ")
		_T("INNER JOIN Artists ON Tracks.artID = Artists.ID) ")
		_T("INNER JOIN Genres ON Tracks.genreID = Genres.ID) ")
		_T("INNER JOIN Collections ON Tracks.collectionID = Collections.ID "),
		len);	
	return _tcslen(sql);
}

//RETURN AND x=1 AND.... (Not "WHERE")

INT TracksViewSchema::GetSQLSorting(TCHAR* sql, int len,
								Sorting sorting,
								BOOL SortDescending)
{
	ASSERT(sql != NULL);
	ASSERT(len > 50);
	ASSERT(sorting >= SORT_Disabled && sorting < SORT_Last);
	if (sorting != SORT_Disabled)
	{
		_sntprintf(sql, len, _T(" ORDER BY %s"), TracksVSString[sorting - 1]);
		if (SortDescending)
			_tcsncat(sql, _T(" DESC"), len - _tcslen(sql));
	}
	return _tcslen(sql);
}




void TracksViewSchema::GetSQL(TCHAR* sql, int len, TracksFilter& Filter, Sorting Sorting, BOOL SortingDESC)
{
	ASSERT(sql != NULL);
	ASSERT(len > 500);
	INT curLen = GetSQLSelect(sql, len);
	ASSERT(curLen);
	_tcscat(sql, _T(" W"));//Keep some chars for "Where"
	curLen+=2;
	//size_t newLen = curLen;
	//newLen += GetSQLTracksFilter(&sql[curLen], len - curLen, Filter);
	size_t newLen = GetSQLTracksFilter(&sql[curLen], len - curLen, Filter);

	//MinNumTracks Filter ... only use it when no AlbumID || ArtistID filters apply (for performance reasons)
	if (Filter.MinNumTracks.val > 0 && Filter.AlbumID.match == NUMM_Disabled && Filter.ArtistID.match == NUMM_Disabled)
	{
		newLen += _sntprintf(&sql[curLen + newLen], len - curLen - newLen,
			_T(" AND Tracks.artID IN ")
			_T("(Select tracks.artID from Tracks group by tracks.artId having count(tracks.artID) < %d)"), 
			Filter.MinNumTracks.val);
	}


	if (newLen != 0)
	//if (newLen != curLen)
	{
		sql[curLen] = 'H';
		sql[curLen+1] = 'E';
		sql[curLen+2] = 'R';
		sql[curLen+3] = 'E';
		curLen += newLen;
	}
	else
	{
		curLen -= 2;
		sql[curLen] = 0;
	}

	GetSQLSorting(&sql[curLen], len - curLen, Sorting, SortingDESC);
}

void TracksViewSchema::GetSQLSimpleSearch(TCHAR* sql, int len,
										  TracksFilter& Filter,
										  LPCTSTR SearchString,
										  Sorting Sorting, BOOL SortDescending)
{

	ASSERT(sql != NULL);
	ASSERT(len > 500);
	ASSERT(Filter.MinNumTracks.match == NUMM_Disabled);
	ASSERT(SearchString != NULL);
	INT curLen = GetSQLSelect(sql, len);
	ASSERT(curLen);
	_tcscat(sql, _T(" W"));//Keep some chars for "Where"
	curLen+=2;
	size_t newLen = curLen;
	newLen += GetSQLTracksFilter(&sql[curLen], len - curLen, Filter);

	if (SearchString[0] != 0)
	{
		TCHAR FixedField[150];
		CDBEngine::FixStringField(SearchString, FixedField, 300);
		newLen += _stprintf(&sql[newLen], _T(" AND (Tracks.Name LIKE '%%%s%%' OR Artists.Name LIKE '%%%s%%' OR Albums.Name LIKE '%%%s%%' OR Tracks.Path LIKE '%%%s%%')"), FixedField, FixedField, FixedField, FixedField);
	}
	
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

	GetSQLSorting(&sql[curLen], len - curLen, Sorting, SortDescending);

}



void TracksViewSchema::GetSQLHistory(TCHAR* sql, int len, 
									 TracksFilter& Filter,
									 Sorting sorting, BOOL SortDescending)
{

	ASSERT(sql != NULL);
	ASSERT(len > 1000);
	_tcsncpy(sql, _T("SELECT ")
		_T("First([Tracks.Rating]),'[' & count(*) & ']' & First([Tracks.Name]),First([Albums.Name]),First([Artists.Name]),")
		_T("First([Tracks.TrackNo]),First([Tracks.Path]),First([Tracks.Size]),First([Tracks.Duration]),")
		_T("First([Tracks.Bitrate]),First([Tracks.Year]),First([Genres.Name]),First([Tracks.DateAdded]),")
		_T("First([Tracks.ID]),First([Tracks.artID]),First([Tracks.albID]),First([Tracks.Type]),")
		_T("First([Tracks.LyrID]),First([Tracks.perID]),First([Tracks.comID]),First([Genres.ID]),")
		_T("First([Tracks.SyncState]), First([Tracks.WriteableState]),First([Collections.Type]),First([Collections.Name]),")
		_T("First([Collections.ID]) ")
		_T("FROM ((((((HistLog INNER JOIN HistTracks ON HistTracks.ID = HistLog.trackID) ")
		_T("INNER JOIN HistArtists ON HistArtists.ID = HistTracks.artID) ")
		_T("INNER JOIN TRACKS ON Tracks.Name = HistTracks.Name) ")
		_T("INNER JOIN Artists ON Tracks.artID = Artists.ID AND Artists.Name = HistArtists.Name) ")
		_T("INNER JOIN Albums ON Tracks.albID = Albums.ID) ")
		_T("INNER JOIN Genres ON Tracks.genreID = Genres.ID) ")
		_T("INNER JOIN Collections ON Tracks.collectionID = Collections.ID ")
		_T("WHERE HistLog.actID=1"),
		len);

	size_t curLen = _tcslen(sql);
	curLen += GetSQLTracksFilter(&sql[curLen], len - curLen, Filter);

	_tcscat(sql, _T(" GROUP BY tracks.Path, HistLog.trackID ORDER BY count(*) DESC"));

	curLen = _tcslen(sql);
	if (sorting != SORT_Disabled)
	{
		_sntprintf(&sql[curLen], len - curLen, _T(" ,first(%s)"), TracksVSString[sorting - 1]);
		if (SortDescending)
			_tcsncat(sql, _T(" DESC"), len - _tcslen(sql));
	}
};


INT TracksViewSchema::GetRating()
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(0, val); 
	return val;
}
void TracksViewSchema::GetName(TCHAR* bf, INT len)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(1, bf, len);
}
//LPCTSTR TracksViewSchema::GetName()
//{
//	ASSERT(m_bRecordsetReady);
//	return m_RS->GetFieldValue(1);
//}
void TracksViewSchema::GetAlbumName(TCHAR* bf, INT len)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(2, bf, len);
}
//LPCTSTR TracksViewSchema::GetAlbumName()
//{
//	ASSERT(m_bRecordsetReady);
//	return m_RS->GetFieldValue(2);
//}
void TracksViewSchema::GetArtistName(TCHAR* bf, INT len)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(3, bf, len);
}
//LPCTSTR TracksViewSchema::GetArtistName()
//{
//	ASSERT(m_bRecordsetReady);
//	return m_RS->GetFieldValue(3);
//}
INT TracksViewSchema::GetTrackNo() 
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(4, val); 
	return val;
}
void TracksViewSchema::GetLocation(TCHAR* bf, INT len)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(5, bf, len);
}
//LPCTSTR TracksViewSchema::GetLocation()
//{
//	ASSERT(m_bRecordsetReady);
//	return m_RS->GetFieldValue(5);
//}
UINT TracksViewSchema::GetSize() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(6, val); 
	return val;
}
UINT TracksViewSchema::GetDuration() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(7, val); 
	return val;
}
INT TracksViewSchema::GetBitRate() 
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(8, val); 
	return val;
}
INT TracksViewSchema::GetYear() 
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(9, val); 
	return val;
}
void TracksViewSchema::GetGenreName(TCHAR* bf, INT len)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(10, bf, len);
}
//LPCTSTR TracksViewSchema::GetGenreName()
//{
//	ASSERT(m_bRecordsetReady);
//	return m_RS->GetFieldValue(10);
//}
void TracksViewSchema::GetDateAdded(SYSTEMTIME& t)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(11, t);
}
void TracksViewSchema::GetDateAdded(TCHAR* bf, INT len, LPCTSTR format)
{
	ASSERT(m_bRecordsetReady);
	SYSTEMTIME t;
	m_RS->GetFieldValue(11, t);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &t, format, bf, len);
}
UINT TracksViewSchema::GetTrackID() {
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(12, val); 
	return val;
}
UINT TracksViewSchema::GetArtID() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(13, val); 
	return val;
}
UINT TracksViewSchema::GetAlbID() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(14, val); 
	return val;
}
INT TracksViewSchema::GetTypeID() //mp3, wma, vi etc..
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(15, val);
	return val;
}
UINT TracksViewSchema::GetLyricsID() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(16, val); 
	return val;
}
UINT TracksViewSchema::GetPersonalID() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(17, val); 
	return val;
}
UINT TracksViewSchema::GetCommentID() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(18, val); 
	return val;
}
UINT TracksViewSchema::GetGenreID() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(19, val); 
	return val;
}

INT TracksViewSchema::GetSyncState()
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(20, val); 
	return val;
}

INT TracksViewSchema::GetWritableState()
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(21, val); 
	return val;

}

BOOL TracksViewSchema::GetFileIsInSync()
{
	return GetSyncState() == TSS_InSync;
}

BOOL TracksViewSchema::GetFileIsWriteble()
{
	return GetWritableState() == TWS_Writable;
}

COLLECTION_TYPES TracksViewSchema::GetCollectionType()
{
	ASSERT(m_bRecordsetReady);
	INT val = 0; 
	m_RS->GetFieldValue(22, val); 
	return (COLLECTION_TYPES) val;
}

void TracksViewSchema::GetCollectionName(TCHAR* bf, INT len)
{
	ASSERT(bf!=NULL);
	ASSERT(len>0);
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(23, bf, len);
}

UINT TracksViewSchema::GetCollectionID() 
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(24, val); 
	return val;
}
