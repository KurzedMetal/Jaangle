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
#ifndef _CHistMainListCtrl_h_
#define _CHistMainListCtrl_h_

//#include "HistMainQuery.h"
#include "HistListCtrl.h"
#include "DataRecords.h"

class CHistMainListCtrl : public CHistListCtrl
{
public:
	enum Columns
	{
		COL_DateAdded,
		COL_TrackName,
		COL_ArtistName
	};

	CHistMainListCtrl();
	virtual ~CHistMainListCtrl()	{}

	void Refresh();
	virtual UINT GetRecordCount()
	{
		return (UINT)m_collection.size();
	}
	void ExecuteTracks(BOOL enqueue);

protected:
	void InsertColumns();
	void GetDispInfo(LV_ITEM* pItem);
	virtual void CreateMainMenu(BCMenu& menu);
	virtual void InitMainMenu(BCMenu& menu);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void ToggleSorting(INT ActiveColum);
	virtual BOOL GetSortingDesc()						{return !m_so.ascending;}
	virtual INT GetSortingColumn();

	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
private:
	FullHistLogRecordCollection m_collection;
	BOOL GetFullTrackRecordCollectionByItemID(FullTrackRecordCollection& col, INT nItem, INT limitRecords);
	SortOption m_so;
};

#endif
