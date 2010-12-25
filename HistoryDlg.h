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
#ifndef _HISTORYDLG_H_
#define _HISTORYDLG_H_

#include "HistoryDlgFilter.h"
#include "afxwin.h"
#include "HistMainListCtrl.h"
#include "HistArtistsListCtrl.h"
#include "HistTracksListCtrl.h"

class CHistoryDlg : public CDialog
{
	DECLARE_DYNAMIC(CHistoryDlg)

public:

	CHistoryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHistoryDlg();

	enum { IDD = IDD_HISTORY };

	void SetArtistFilterText(LPCTSTR artist, BOOL bAutoSwitch = FALSE);
	void SetTrackFilterText(LPCTSTR track, LPCTSTR artist, BOOL bAutoSwitch = FALSE);
	
	enum TABS
	{
		TAB_History,
		TAB_TopTracks,
		TAB_TopArtists,
		TAB_Last
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
	enum TimeIntervals
	{
		TI_First,
		TI_Disabled,
		TI_Year,
		TI_Month,
		TI_Day,
		TI_Last365Days,
		TI_Last30Days,
		TI_Last10Days,
		TI_Last24Hours,
		TI_Last
	};

	CTabCtrl m_Tab;
	CComboBox m_cmbDate;

	CHistMainListCtrl m_HMainList;
	CHistTracksListCtrl m_HTracksList;
	CHistArtistsListCtrl m_HArtistsList;

	//CHistListCtrl* m_List[TAB_Last];
	CStatusBar m_StatusBar;
	CEdit m_edtTrack;
	CEdit m_edtArtist;
	//CSpinButtonCtrl m_spinDate;
	CSliderCtrl m_sldDate;

	void ChangeTime(SYSTEMTIME& st, TimeIntervals ti, INT val, BOOL SetStart = FALSE);
	void UpdateStatusBar(DWORD LastTickCount, UINT RecordCount);
	INT TimeDiff(SYSTEMTIME& first, SYSTEMTIME& last, TimeIntervals ti);
	void InitTab();
	void RefreshView();
	void DelayedRefreshView();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnEnChangeEdtArtist();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnEnChangeEdtTrack();
	afx_msg void OnCbnSelchangeCmbdate();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//BOOL m_bHiddenViewNeedsRefresh;

	//SYSTEMTIME m_MinTime;
	//SYSTEMTIME m_MaxTime;
	SYSTEMTIME m_FirstDate;
	HistoryDlgFilter m_dlgFilter;
	INT m_CurrentDatePos;
	HICON m_hIcon;
	BOOL m_bRefreshDisabled;


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};

#endif
