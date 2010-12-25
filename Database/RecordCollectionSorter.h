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
#ifndef _RecordCollectionSorter_h_
#define _RecordCollectionSorter_h_

#include "DataRecords.h"

enum RecordSortOptionsEnum
{
	RSO_None,
	RSO_Title,
	RSO_TrackNo,
	RSO_Album,
	RSO_Artist,
	RSO_Rating,
	RSO_Location,
	RSO_Size,
	RSO_Duration,
	RSO_Bitrate,
	RSO_Year,
	RSO_Genre,
	RSO_Collection,
	RSO_DateAdded,
	RSO_TrackCount,
	RSO_Hits,
	RSO_FirstDate,
	RSO_LastDate,
	RSO_Last
};
struct SortOption
{
	SortOption():option(RSO_None), ascending(FALSE)									{}
	SortOption(RecordSortOptionsEnum opt, BOOL asc):option(opt), ascending(asc)		{}
	bool operator==(const SortOption &l) const	{
		return (option == l.option) && (ascending == l.ascending);}
	bool operator!=(const SortOption &l) const	{
		return !(*this == l);}
	RecordSortOptionsEnum option;
	BOOL ascending;
};
class SortOptionCollection
{
public:
	//Appends a new sorting option. If the sort options then it applies the new ascending attribute 
	void ApplySortOption(SortOption option);
	UINT GetSortOptionsCount();
	void ClearSortOptions();
	SortOption GetSortOption(UINT idx);
	void ReverseSortOption(UINT idx);

	//BOOL LoadFromString(LPCTSTR soInfo);
	//BOOL SaveToString(LPTSTR soInfo, UINT bfLen);
private:
	std::vector<SortOption> m_sortOptions;
};

class FullTrackComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	FullTrackComparison():m_pSOC(NULL)								{}
	FullTrackComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(FullTrackRecordSP a, FullTrackRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class FullArtistComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	FullArtistComparison():m_pSOC(NULL)								{}
	FullArtistComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(FullArtistRecordSP a, FullArtistRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class FullAlbumComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	FullAlbumComparison():m_pSOC(NULL)								{}
	FullAlbumComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(FullAlbumRecordSP a, FullAlbumRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class GenreComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	GenreComparison():m_pSOC(NULL)								{}
	GenreComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(GenreRecordSP a, GenreRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class CollectionComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	CollectionComparison():m_pSOC(NULL)								{}
	CollectionComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(CollectionRecordSP a, CollectionRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class YearComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	YearComparison():m_pSOC(NULL)								{}
	YearComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(YearRecordSP a, YearRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class MonthAddedComparison
{
public:
	// constructor which takes a reference to a vector of employees
	MonthAddedComparison():m_pSOC(NULL)								{}
	MonthAddedComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(MonthAddedRecordSP a, MonthAddedRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class FullHistLogComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	FullHistLogComparison():m_pSOC(NULL)								{}
	FullHistLogComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(FullHistLogRecordSP a, FullHistLogRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class FullHistTrackComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	FullHistTrackComparison():m_pSOC(NULL)								{}
	FullHistTrackComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(FullHistTrackRecordSP a, FullHistTrackRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class FullHistArtistComparison 
{
public:
	// constructor which takes a reference to a vector of employees
	FullHistArtistComparison():m_pSOC(NULL)								{}
	FullHistArtistComparison(SortOptionCollection* pSOC):m_pSOC(pSOC)	{}
	void SetSortOptionCollection(SortOptionCollection* pSOC)		{m_pSOC = pSOC;}
	bool operator()(FullHistArtistRecordSP a, FullHistArtistRecordSP b) const;
private:
	SortOptionCollection* m_pSOC;
};

class RecordCollectionSorter
{
public:
	static void SortFullTrackRecordCollection(FullTrackRecordCollection& col, SortOptionCollection& comp);
	static void SortFullArtistRecordCollection(FullArtistRecordCollection& col, SortOptionCollection& comp);
	static void SortFullAlbumRecordCollection(FullAlbumRecordCollection& col, SortOptionCollection& comp);
	static void SortGenreRecordCollection(GenreRecordCollection& col, SortOptionCollection& comp);
	static void SortCollectionRecordCollection(CollectionRecordCollection& col, SortOptionCollection& comp);
	static void SortYearRecordCollection(YearRecordCollection& col, SortOptionCollection& comp);
	static void SortMonthAddedRecordCollection(MonthAddedRecordCollection& col, SortOptionCollection& sortOptions);

	static void SortFullHistLogRecordCollection(FullHistLogRecordCollection& col, SortOptionCollection& comp);
	static void SortFullHistTrackRecordCollection(FullHistTrackRecordCollection& col, SortOptionCollection& comp);
	static void SortFullHistArtistRecordCollection(FullHistArtistRecordCollection& col, SortOptionCollection& comp);
};

#endif

