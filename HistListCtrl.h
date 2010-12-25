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
#ifndef _CHistListCtrl_h_
#define _CHistListCtrl_h_

#include "GuiControls/BCMenu.h"
#include "HistoryDlgFilter.h"
#include "RecordCollectionSorter.h"

class CHistoryDlg;

#define ID_EXPORTSELECTION 600
#define ID_EXPORTALL 601


class CHistListCtrl : public CListCtrl
{
	//DECLARE_DYNAMIC(CHistMainListCtrl)

public:
	enum TimeIntervals
	{
		TI_First,
		TI_Disabled,
		TI_Year,
		TI_Month,
		TI_Week,
		TI_Day,
		TI_Last365Days,
		TI_Last30Days,
		TI_Last10Days,
		TI_Last24Hours,
		TI_Last
	};
	CHistListCtrl();
	virtual ~CHistListCtrl();

	void Init(HistoryDlgFilter* filter);
	//void PrepareRanking(HistArtistsViewSchema* schema);
	void SetHistoryDlg(CHistoryDlg* pHistDlg)			
	{
		ASSERT(pHistDlg); 
		m_pHistDlg = pHistDlg;
	}

	virtual void Refresh()						= 0;
	virtual UINT GetRecordCount()				= 0;

protected:
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	DECLARE_MESSAGE_MAP()

	virtual void InsertColumns()				= 0;
	virtual void GetDispInfo(LV_ITEM* pItem)	= 0;
	virtual void CreateMainMenu(BCMenu& menu)	= 0;
	virtual void InitMainMenu(BCMenu& menu)		= 0;

	HistoryDlgFilter* m_pFilter;
	CHistoryDlg* m_pHistDlg;
private:
	BCMenu m_MainMenu;

protected:
	virtual void ToggleSorting(INT ActiveColumn)		= 0;
	virtual BOOL GetSortingDesc()						= 0;
	virtual INT GetSortingColumn()						= 0;

};

#endif
