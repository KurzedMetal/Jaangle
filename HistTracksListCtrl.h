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
#ifndef _CHistTracksListCtrl_h_
#define _CHistTracksListCtrl_h_

//#include "GuiControls/BCMenu.h"
//#include "HistTracksViewSchema.h"
//#include "HistTracksQuery.h"
#include "HistListCtrl.h"
#include "SQLManager.h"

class CHistTracksListCtrl : public CHistListCtrl
{
	//DECLARE_DYNAMIC(CHistMainListCtrl)

public:
	enum Columns
	{ 
		COL_Rank,
		COL_Hits,
		COL_TrackName,
		COL_ArtistName,
		COL_FirstDate,
		COL_LastDate
	};

	CHistTracksListCtrl();
	virtual ~CHistTracksListCtrl()	{}


	void Refresh();
	virtual UINT GetRecordCount()//	{return m_Schema.GetRecordCount();}
	{
		return m_collection.size();
	}

protected:
	void InsertColumns();
	void GetDispInfo(LV_ITEM* pItem);
	INT GetSorting(INT column);
	virtual void CreateMainMenu(BCMenu& menu);
	virtual void InitMainMenu(BCMenu& menu);
	virtual void ToggleSorting(INT ActiveColumn);
	virtual BOOL GetSortingDesc()				{return !m_so.ascending;}
	virtual INT GetSortingColumn();


private:
	void ExecuteTracks(BOOL enqueue);
	void PrepareRanking();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL GetFullTrackRecordCollectionByItemID(FullTrackRecordCollection& col, INT nItem, INT limitRecords);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
	SortOption m_so;
	FullHistTrackRecordCollection m_collection;

	UINT GetRankingForHits(UINT hits);
	std::vector<SQLManager::Rank> m_ranking;

};

#endif
