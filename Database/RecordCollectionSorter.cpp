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
#include "RecordCollectionSorter.h"

#include <algorithm>

//-------------------------------------------------------------------------
//---------------------------Global functions------------------------------
//-------------------------------------------------------------------------
inline int NumComparison(int num1, int num2)
{
	return num1 > num2 ? 1 : ((num1 == num2) ? 0 : -1);
}

inline int DateComparison(SYSTEMTIME& date1, SYSTEMTIME& date2)
{
	int ret = NumComparison(date1.wYear, date2.wYear);
	if (ret != 0)	return ret;
	ret = NumComparison(date1.wMonth, date2.wMonth);
	if (ret != 0)	return ret;
	ret = NumComparison(date1.wDay, date2.wDay);
	if (ret != 0)	return ret;
	ret = NumComparison(date1.wHour, date2.wHour);
	if (ret != 0)	return ret;
	ret = NumComparison(date1.wMinute, date2.wMinute);
	if (ret != 0)	return ret;
	ret = NumComparison(date1.wSecond, date2.wSecond);
	if (ret != 0)	return ret;
	return NumComparison(date1.wMilliseconds, date2.wMilliseconds);
}

//-------------------------------------------------------------------------
//-----------------SortOptionCollection------------------------------------
//-------------------------------------------------------------------------

void SortOptionCollection::ApplySortOption(SortOption option)
{
	std::vector<SortOption>::iterator it = m_sortOptions.begin();
	for (;it!= m_sortOptions.end(); it++)
	{
		if (it->option == option.option)
		{
			it->ascending = option.ascending;
			return;
		}
	}
	m_sortOptions.push_back(option);
}
SortOption SortOptionCollection::GetSortOption(UINT idx)
{
	ASSERT(idx < GetSortOptionsCount());
	return m_sortOptions[idx];
	//if (idx < GetSortOptionsCount())
	//	return m_sortOptions[idx];
	//return SortOption(RSO_None, TRUE);
}
void SortOptionCollection::ReverseSortOption(UINT idx)
{
	ASSERT(idx < GetSortOptionsCount());
	m_sortOptions[idx].ascending = !m_sortOptions[idx].ascending;
	//if (idx < GetSortOptionsCount())
	//m_sortOptions[idx].ascending = !m_sortOptions[idx].ascending;
}
UINT SortOptionCollection::GetSortOptionsCount()
{
	return m_sortOptions.size();
}
void SortOptionCollection::ClearSortOptions()
{
	m_sortOptions.clear();
}

//BOOL SortOptionCollection::LoadFromString(LPCTSTR soInfo)
//{
//	ClearSortOptions();
//	TCHAR bf[300];
//	_tcsncpy(bf, soInfo, 300);
//	LPTSTR tok = _tcstok(bf, _T(":"));
//	while (tok != NULL)
//	{
//		INT tokNum = _ttoi(tok);
//		SortOption so(RSO_None, TRUE);
//		so.option = RecordSortOptionsEnum(tokNum > 0 ? tokNum : -tokNum);
//		so.ascending = tokNum > 0;
//		ApplySortOption(so);
//		tok = _tcstok(NULL, _T(":"));
//	}
//	return TRUE;
//
//}
//BOOL SortOptionCollection::SaveToString(LPTSTR soInfo, UINT soInfoLen)
//{
//	ASSERT(soInfoLen >= 300);
//	soInfo[0] = 0;
//	INT bfLen = 0;
//	for (UINT i =0; i<GetSortOptionsCount(); i++)
//	{
//		SortOption so = GetSortOption(i);
//		INT opt = INT(so.option);
//		if (!so.ascending)
//			opt = -opt;
//		bfLen += _sntprintf(&soInfo[bfLen], soInfoLen-bfLen, _T("%d:"), opt);
//	}
//	if (bfLen > 0)
//		soInfo[bfLen - 1] = 0;
//	return TRUE;
//}


//-------------------------------------------------------------------------
//-----------------FullTrackComparison------------------------------------
//-------------------------------------------------------------------------

bool FullTrackComparison::operator()(FullTrackRecordSP a, FullTrackRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
			cmpret = _tcsicmp(a->track.name.c_str(), b->track.name.c_str());
			break;
		case RSO_Album:	
			cmpret = _tcsicmp(a->album.name.c_str(), b->album.name.c_str());
			break;
		case RSO_Artist:
			cmpret = _tcsicmp(a->artist.name.c_str(), b->artist.name.c_str());
			break;
		case RSO_Genre:
			cmpret = _tcsicmp(a->genre.name.c_str(), b->genre.name.c_str());
			break;
		case RSO_TrackNo:
			cmpret = NumComparison(a->track.trackNo, b->track.trackNo);
			break;
		case RSO_Size:
			cmpret = NumComparison(a->track.size, b->track.size);;
			break;
		case RSO_Duration:
			cmpret = NumComparison(a->track.duration, b->track.duration);
			break;
		case RSO_Bitrate:
			cmpret = NumComparison(a->track.bitrate, b->track.bitrate);
			break;
		case RSO_DateAdded:
			cmpret = DateComparison(a->track.dateAdded, b->track.dateAdded);
			break;
		case RSO_Location:
			cmpret = _tcsicmp(a->track.location.c_str(), b->track.location.c_str());
			break;
		case RSO_Rating:
			cmpret = NumComparison(a->track.rating, b->track.rating);
			break;
		case RSO_Year:
			cmpret = NumComparison(a->track.year, b->track.year);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool FullArtistComparison::operator()(FullArtistRecordSP a, FullArtistRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
		case RSO_Artist:
			cmpret = _tcsicmp(a->artist.name.c_str(), b->artist.name.c_str());
			break;
		case RSO_Genre:
			cmpret = _tcsicmp(a->genre.name.c_str(), b->genre.name.c_str());
			break;
		case RSO_Rating:
			cmpret = NumComparison(a->artist.rating, b->artist.rating);
			break;
		case RSO_TrackCount:
			cmpret = NumComparison(a->artist.trackCount, b->artist.trackCount);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool FullAlbumComparison::operator()(FullAlbumRecordSP a, FullAlbumRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
		case RSO_Album:
			cmpret = _tcsicmp(a->album.name.c_str(), b->album.name.c_str());
			break;
		case RSO_Artist:
			cmpret = _tcsicmp(a->artist.name.c_str(), b->artist.name.c_str());
			break;
		case RSO_Year:
			cmpret = NumComparison(a->album.year, b->album.year);
			break;
		case RSO_Rating:
			cmpret = NumComparison(a->album.rating, b->album.rating);
			break;
		case RSO_TrackCount:
			cmpret = NumComparison(a->album.trackCount, b->album.trackCount);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool GenreComparison::operator()(GenreRecordSP a, GenreRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
		case RSO_Genre:
			cmpret = _tcsicmp(a->name.c_str(), b->name.c_str());
			break;
		case RSO_TrackCount:
			cmpret = NumComparison(a->trackCount, b->trackCount);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool CollectionComparison::operator()(CollectionRecordSP a, CollectionRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
		case RSO_Collection:
			cmpret = _tcsicmp(a->name.c_str(), b->name.c_str());
			break;
		case RSO_TrackCount:
			cmpret = NumComparison(a->trackCount, b->trackCount);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool YearComparison::operator()(YearRecordSP a, YearRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
		case RSO_Year:
			cmpret = NumComparison(a->year, b->year);
			break;
		case RSO_TrackCount:
			cmpret = NumComparison(a->trackCount, b->trackCount);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool MonthAddedComparison::operator()(MonthAddedRecordSP a, MonthAddedRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
		case RSO_Year:
			cmpret = NumComparison(a->month, b->month);
			break;
		case RSO_TrackCount:
			cmpret = NumComparison(a->trackCount, b->trackCount);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}



bool FullHistLogComparison::operator()(FullHistLogRecordSP a, FullHistLogRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Title:
			cmpret = _tcsicmp(a->track.name.c_str(), b->track.name.c_str());
			break;
		case RSO_DateAdded:
			cmpret = DateComparison(a->log.dateAdded, b->log.dateAdded);
			break;
		case RSO_Artist:
			cmpret =_tcsicmp(a->artist.name.c_str(), b->artist.name.c_str());
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool FullHistTrackComparison::operator()(FullHistTrackRecordSP a, FullHistTrackRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Hits:
			cmpret = NumComparison(a->hits, b->hits);
			break;
		case RSO_Title:
			cmpret = _tcsicmp(a->track.name.c_str(), b->track.name.c_str());
			break;
		case RSO_Artist:
			cmpret =_tcsicmp(a->artist.name.c_str(), b->artist.name.c_str());
			break;
		case RSO_FirstDate:
			cmpret = DateComparison(a->firstTime, b->firstTime);
			break;
		case RSO_LastDate:
			cmpret = DateComparison(a->lastTime, b->lastTime);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}

bool FullHistArtistComparison::operator()(FullHistArtistRecordSP a, FullHistArtistRecordSP b) const
{
	if (m_pSOC == NULL)
		return false;
	UINT sortCount = m_pSOC->GetSortOptionsCount();
	for (UINT i = 0; i<sortCount; i++)
	{
		SortOption so = m_pSOC->GetSortOption(i);
		INT cmpret = 0;
		switch (so.option)
		{
		case RSO_Hits:
			cmpret = NumComparison(a->hits, b->hits);
			break;
		case RSO_Artist:
			cmpret = _tcsicmp(a->artist.name.c_str(), b->artist.name.c_str());
			break;
		case RSO_FirstDate:
			cmpret = DateComparison(a->firstTime, b->firstTime);
			break;
		case RSO_LastDate:
			cmpret = DateComparison(a->lastTime, b->lastTime);
			break;
		default:
			//ASSERT(0);
			break;
		}
		if (cmpret != 0)
		{
			if (so.ascending)
				cmpret = - cmpret;
			return cmpret > 0;
		}
	}
	return false;
}


void RecordCollectionSorter::SortFullTrackRecordCollection(
	FullTrackRecordCollection& col, SortOptionCollection& sortOptions)
{
	FullTrackComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}

void RecordCollectionSorter::SortFullArtistRecordCollection(
	FullArtistRecordCollection& col, SortOptionCollection& sortOptions)
{
	FullArtistComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}

void RecordCollectionSorter::SortFullAlbumRecordCollection(
	FullAlbumRecordCollection& col, SortOptionCollection& sortOptions)
{
	FullAlbumComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}

void RecordCollectionSorter::SortGenreRecordCollection(
	GenreRecordCollection& col, SortOptionCollection& sortOptions)
{
	GenreComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}

void RecordCollectionSorter::SortCollectionRecordCollection(
	CollectionRecordCollection& col, SortOptionCollection& sortOptions)
{
	CollectionComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}

void RecordCollectionSorter::SortYearRecordCollection(
	YearRecordCollection& col, SortOptionCollection& sortOptions)
{
	YearComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}
void RecordCollectionSorter::SortMonthAddedRecordCollection(
	MonthAddedRecordCollection& col, SortOptionCollection& sortOptions)
{
	MonthAddedComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}


void RecordCollectionSorter::SortFullHistLogRecordCollection(
	FullHistLogRecordCollection& col, SortOptionCollection& sortOptions)
{
	FullHistLogComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}

void RecordCollectionSorter::SortFullHistTrackRecordCollection(
	FullHistTrackRecordCollection& col, SortOptionCollection& sortOptions)
{
	FullHistTrackComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}

void RecordCollectionSorter::SortFullHistArtistRecordCollection(
	FullHistArtistRecordCollection& col, SortOptionCollection& sortOptions)
{
	FullHistArtistComparison comp(&sortOptions);
	std::sort(col.begin(), col.end(), comp);
}
