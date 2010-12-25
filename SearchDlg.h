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
#ifndef _CSearchDlg_h_
#define _CSearchDlg_h_

#include "MainListCtrl.h"
#include "SimpleSearchCHdlg.h"
#include "DetailedSearchCHdlg.h"
#include "LatestAdditionsPP.h"
#include "RatingPP.h"
//#include "SpecialInfoPP.h"
#include "FullTextSearchCHdlg.h"
//#include "TitledListBox.h"
#include "GuiControls/BCMenu.h"
#include "TSTranslatable.h"
#include "FullTextListCtrl.h"

const TCHAR* const sShowAllTracksCode = _T("*");

class AppSettings;
// CSearchDlg dialog
#define WM_PAGEUPDATED WM_USER + 200

class CSearchDlg : public CDialog, public TSTranslatable
{
	DECLARE_DYNAMIC(CSearchDlg)
public:
	CSearchDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchDlg();

// Dialog Data
	enum { IDD = IDD_SEARCH };
	void LoadOptions();
	void SaveOptions();
	void SetSimpleSearchText(LPCTSTR searchString);
	virtual void ApplyTranslation(ITranslation& skin); //ICtrlStrings

protected:
	//afx_msg void OnEnChangeEdtSimple();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnTcnSelchangeTabSearch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	//afx_msg void OnBnClickedCmdChange();
	afx_msg LRESULT OnPageUpdated(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	DECLARE_MESSAGE_MAP()
private:
	void RefreshMainList();
	void PlayListBoxTrack(BOOL bEnqueue);
private:
	enum TabID
	{
		TAB_First = 0,
		TAB_SimpleSearch,
		TAB_DetailedSearch,
		TAB_LatestTracks,
		TAB_BestWorstTracks,
		TAB_FullText,
		TAB_All,
		TAB_Last
	};

	//CTabCtrl m_SearchTab;
	CMainListCtrl m_SearchLst;
	CStatusBar m_StatusBar;
	HICON m_hIcon;
	//CTitledListBox m_FullTextList;
	CFullTextListCtrl m_FullTextListCtrl;


	CPropertySheet m_Sheet;
	CSimpleSearchCHdlg m_SimpleSearchPP;
	CDetailedSearchCHdlg m_DetailedSearchPP;
	CLatestAdditionsPP m_LatestAdditionsPP;
	CRatingPP m_RatingPP;
	//CSpecialInfoPP m_SpecialInfoPP;
	CFullTextSearchCHdlg m_FullTextPP;

	BCMenu m_ListBoxMenu;
	BOOL m_bInitialized;

	BOOL LoadState(AppSettings* pAppSettings);
	BOOL SaveState(AppSettings* pAppSettings);


public:
	afx_msg void OnDestroy();
};

#endif
