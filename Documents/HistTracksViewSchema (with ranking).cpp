#include "StdAfx.h"
#include "HistTracksViewSchema.h"
#include 

const TCHAR* const HistTracksVSString[]  = {
	_T("FDate"),
	_T("LDate"),
	_T("TrackName"),
	_T("ArtistName"),
	_T("Hits"),
	_T("Hits") //Maybe Should be Rank but...
};


void HistTracksViewSchemaHelper::GetSQL(TCHAR* sql, UINT len, 
				   SYSTEMTIME* startDate = NULL,
				   SYSTEMTIME* endDate = NULL,
				   TCHAR* ArtistFilter = NULL,
				   TCHAR* TrackFilter = NULL,
				   UINT actID = 1,
				   UINT LimitRecords = 0,
				   Sorting sorting = SORT_Hits,
				   BOOL SortDescending = TRUE)
{
	_tcsncpy(sql, 
		_T("SELECT Count(*) AS Hits, HistLog.trackID, Min(HistLog.DateAdded) AS FirstDate,")
		_T("Max(HistLog.DateAdded) AS LastDate FROM HistLog  "), len);
	size_t curLen = _tcslen(sql);
	GetSQLWhere(&sql[curLen], len - curLen,
		startDate,
		endDate,
		ArtistFilter,
		TrackFilter,
		actID);
	curLen = _tcslen(sql);
	_tcsncat(sql, _T("GROUP BY HistLog.trackID"), len - curLen);

}


void HistTracksViewSchema::GetSQL(TCHAR* sql, UINT len, 
		SYSTEMTIME* startDate/* = NULL*/,
		SYSTEMTIME* endDate/* = NULL*/,
		TCHAR* ArtistFilter/* = NULL*/,
		TCHAR* TrackFilter/* = NULL*/,
		UINT actID/* = 0*/,
		UINT LimitRecords/* = 0*/,
		Sorting sorting/* = SORT_DateAdded*/,
		BOOL SortDescending /*= TRUE*/)
{
	ASSERT(sql != NULL);
	ASSERT(len > 800);
	ASSERT(sorting >= SORT_Disabled && sorting < SORT_Last);

	//SELECT g1.Hits, g1.TrackID, g1.FirstDate, g1.LastDate, HistTracks.Name, (select count(*)+1  from 
	//(
	//SELECT Count(*) AS Hits, HistLog.trackID, Min(HistLog.DateAdded) AS FirstDate, Max(HistLog.DateAdded) AS LastDate
	//FROM HistLog
	//WHERE (((HistLog.actID)=1))
	//GROUP BY HistLog.trackID
	//)
	// as g2  where g2.Hits > g1.Hits) AS Rank
	//FROM 
	//(
	//SELECT Count(*) AS Hits, HistLog.trackID, Min(HistLog.DateAdded) AS FirstDate, Max(HistLog.DateAdded) AS LastDate
	//FROM HistLog
	//WHERE (((HistLog.actID)=1))
	//GROUP BY HistLog.trackID
	//)
	//AS g1 INNER JOIN HistTracks ON g1.trackID=HistTracks.ID
	//ORDER BY g1.Hits DESC;

	
	//Build TopTracksViewHelper
	const int sqSQLLen = 300;
	TCHAR sqSQL[sqSQLLen];
/*	_tcsncpy(sqSQL, 
		_T("SELECT Count(*) AS Hits, HistLog.trackID, Min(HistLog.DateAdded) AS FirstDate,")
		_T("Max(HistLog.DateAdded) AS LastDate FROM HistLog  "), sqSQLLen);
	size_t curLen = _tcslen(sqSQL);
	GetSQLWhere(&sqSQL[curLen], sqSQLLen - curLen,
				startDate,
				endDate,
				ArtistFilter,
				TrackFilter,
				actID);
	curLen = _tcslen(sqSQL);
	_tcsncat(sqSQL, _T("GROUP BY HistLog.trackID"), sqSQLLen - curLen);*/
	_tcsncpy(sqSQL, _T("TopTracksHelper")

	
	


	//Build Main Query
	_tcsncpy(sql, _T("SELECT "), len);
	curLen = _tcslen(sql);
	if (LimitRecords > 0)
		_sntprintf(&sql[curLen],len - curLen, _T("TOP %d "), LimitRecords);
	curLen = _tcslen(sql);

	_sntprintf(sql, len - curLen,
		_T("SELECT g1.Hits as Hits, g1.TrackID as ItemID, g1.FirstDate as FDate, ")
		_T("g1.LastDate as LDate, HistTracks.Name as TrackName, HistArtists.Name as ArtistName, ")
		_T("HistArtists.ID as ItemArtID, ")
		_T("(select count(*)+1  from (%s) as g2  where g2.Hits > g1.Hits) AS Rank ")
		_T("FROM (%s) AS g1 INNER JOIN ")
		_T("(HistTracks INNER JOIN HistArtists ON HistTRacks.artID=HistArtists.ID) ")
		_T("ON g1.trackID=HistTracks.ID ")
		, sqSQL, sqSQL);

	if (sorting != SORT_Disabled)
	{
		curLen = _tcslen(sql);
		_sntprintf(&sql[curLen], len - curLen, _T(" ORDER BY %s"), HistTracksVSString[sorting - SORT_Disabled]);
		if (SortDescending)
		{
			curLen = _tcslen(sql);
			_tcsncat(sql, _T(" DESC"), len - curLen);
		}
	}

}

void HistTracksViewSchema::GetFirstAppear(SYSTEMTIME& t)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(2, t);
}
void HistTracksViewSchema::GetLastAppear(SYSTEMTIME& t)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(3, t);
}
void HistTracksViewSchema::GetName(TCHAR* bf, INT len)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(4, bf, len);
}
void HistTracksViewSchema::GetArtistName(TCHAR* bf, INT len)
{
	ASSERT(m_bRecordsetReady);
	m_RS->GetFieldValue(5, bf, len);
}
UINT HistTracksViewSchema::GetTrackID()
{
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(1, val); 
	return val;
}
UINT HistTracksViewSchema::GetArtID()
{	
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(6, val); 
	return val;
}
UINT HistTracksViewSchema::GetHits()
{	
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(0, val); 
	return val;
}
UINT HistTracksViewSchema::GetRank()
{	
	ASSERT(m_bRecordsetReady);
	ULONG val = 0; 
	m_RS->GetFieldValue(7, val); 
	return val;
}