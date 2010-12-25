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
#include "TeenSpirit.h"
#include "HistoryDlg.h"
#include "HistLogTableSchema.h"


// CHistoryDlg dialog

IMPLEMENT_DYNAMIC(CHistoryDlg, CDialog)

CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistoryDlg::IDD, pParent),
	m_bHiddenViewNeedsRefresh(FALSE),
	m_CurrentDatePos(0)
{

}

CHistoryDlg::~CHistoryDlg()
{
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_CMBDATE, m_cmbDate);
	DDX_Control(pDX, IDC_EDT_TRACK, m_edtTrack);
	DDX_Control(pDX, IDC_EDT_ARTIST, m_edtArtist);
	DDX_Control(pDX, IDC_SLIDER, m_sldDate);
	DDX_Control(pDX, IDC_LISTTOPART, m_ListTopArt);
	DDX_Control(pDX, IDC_LISTTOPTRA, m_ListTopTra);
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_RESET, &CHistoryDlg::OnBnClickedBtnReset)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_EN_CHANGE(IDC_EDT_ARTIST, &CHistoryDlg::OnEnChangeEdtArtist)
	ON_EN_CHANGE(IDC_EDT_TRACK, &CHistoryDlg::OnEnChangeEdtTrack)
	ON_CBN_SELCHANGE(IDC_CMBDATE, &CHistoryDlg::OnCbnSelchangeCmbdate)
	ON_WM_HSCROLL()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CHistoryDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CHistoryDlg message handlers

void CHistoryDlg::OnBnClickedBtnReset()
{

	// TODO: Add your control notification handler code here
}

BOOL CHistoryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitTab();
	m_cmbDate.AddString(_T("All Time"));
	m_cmbDate.AddString(_T("Years"));
	m_cmbDate.AddString(_T("Months"));
	m_cmbDate.AddString(_T("Days"));
	m_cmbDate.AddString(_T("365 Days"));
	m_cmbDate.AddString(_T("30 Days"));
	m_cmbDate.AddString(_T("10 Days"));
	m_cmbDate.AddString(_T("24 Hours"));
	m_cmbDate.SetCurSel(0);
	UpdateFirstDate();
	m_List.Init(this);
	m_ListTopTra.Init(this);
	//m_ListTopArt.Init(this);
	CRect rc;
	GetClientRect(&rc);
	OnSize(0, rc.Width(), rc.Height());
	RefreshView();

	return TRUE;
}

void CHistoryDlg::InitTab()
{
	TCITEM tabItem;
	memset(&tabItem, 0, sizeof(TCITEM));
	tabItem.mask = TCIF_TEXT | TCIF_PARAM;
	tabItem.cchTextMax = 50;
	tabItem.pszText = _T("Recorded History");
	m_Tab.InsertItem(TAB_History, &tabItem);
	tabItem.pszText = _T("Top Songs");
	m_Tab.InsertItem(TAB_TopTracks, &tabItem);
	tabItem.pszText = _T("Top Artists");
	m_Tab.InsertItem(TAB_TopArtists, &tabItem);
}
void CHistoryDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rc;
	const int marginX = 5;
	const int marginY = 5;
	const int DistFromLeft = 200;
	const int TabHeight = 20;
	if (IsWindow(m_Tab.m_hWnd))
	{
		m_Tab.SetWindowPos(NULL,	
			DistFromLeft + marginX,		
			marginY,
			cx - DistFromLeft - 2 * marginX,
			cy - 2 * marginY,
			SWP_NOZORDER);
		INT lleft = DistFromLeft + 2 * marginX;
		INT ltop = TabHeight + 2 * marginY;
		INT lcx = cx - DistFromLeft - 4 * marginX;
		INT lcy = cy - TabHeight - 4 * marginY;
		m_List.SetWindowPos(NULL, lleft, ltop, lcx, lcy, SWP_NOZORDER);
		m_ListTopArt.SetWindowPos(NULL, lleft, ltop, lcx, lcy, SWP_NOZORDER);
		m_ListTopTra.SetWindowPos(NULL, lleft, ltop, lcx, lcy, SWP_NOZORDER);
	}
}

void CHistoryDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(368, 249);
	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CHistoryDlg::GetMainViewScema(HistMainViewSchema* schema , 
								   HistMainViewSchema::Sorting Sorting /*= HistMainViewSchema::SORT_Disabled*/, 
								   BOOL SortingDESC /*= TRUE*/)
{
	ASSERT(schema != NULL);
	CDBEngine* eng = ObjFactory::Instance()->GetDbEngine();
	ASSERT(eng != NULL);
	TCHAR bf[2000];

	BOOL retVal = TRUE;
	DWORD tick = GetTickCount();
	ASSERT(m_Tab.GetCurSel() == 0);
	//UpdateData(TRUE);
	TCHAR artFilter[100];
	TCHAR traFilter[100];
	m_edtArtist.GetWindowText(artFilter, 100);
	m_edtTrack.GetWindowText(traFilter, 100);
	int CurDateSel = m_cmbDate.GetCurSel() + 1;
	if (CurDateSel != TI_Disabled)
		HistMainViewSchema::GetSQL(bf, 2000, &m_MinTime, 
			m_CurrentDatePos ? &m_MaxTime : NULL, 
			artFilter, 
			traFilter, 0, 0, Sorting, SortingDESC);
	else
		HistMainViewSchema::GetSQL(bf, 2000, NULL, NULL, artFilter, traFilter, 0, 0, Sorting, SortingDESC);
	schema->QueryDatabase(eng, bf, FALSE);
	return retVal;

}

BOOL CHistoryDlg::GetArtistsViewScema(HistArtistsViewSchema* schema , HistArtistsViewSchema::Sorting Sorting, BOOL SortingDESC)
{
	ASSERT(schema != NULL);
	CDBEngine* eng = ObjFactory::Instance()->GetDbEngine();
	ASSERT(eng != NULL);
	TCHAR bf[2000];

	BOOL retVal = TRUE;
	DWORD tick = GetTickCount();
	ASSERT(m_Tab.GetCurSel() == 0);
	//UpdateData(TRUE);
	TCHAR artFilter[100];
	TCHAR traFilter[100];
	m_edtArtist.GetWindowText(artFilter, 100);
	//m_edtTrack.GetWindowText(traFilter, 100);
	//int CurDateSel = m_cmbDate.GetCurSel() + 1;
	//if (CurDateSel != TI_Disabled)
	//	HistArtistsViewSchema::GetSQL(bf, 2000, &m_MinTime, 
	//		m_CurrentDatePos ? &m_MaxTime : NULL, 
	//		artFilter, 
	//		traFilter);
	//else
	//	HistArtistsViewSchema::GetSQL(bf, 2000, NULL, NULL, artFilter, traFilter);
	schema->QueryDatabase(eng, bf, FALSE);
	return retVal;

}

BOOL CHistoryDlg::GetTracksViewScema(HistTracksViewSchema* schema , HistTracksViewSchema::Sorting Sorting, BOOL SortingDESC)
{
	ASSERT(schema != NULL);
	CDBEngine* eng = ObjFactory::Instance()->GetDbEngine();
	ASSERT(eng != NULL);
	TCHAR bf[2000];

	BOOL retVal = TRUE;
	DWORD tick = GetTickCount();
	TCHAR artFilter[100];
	TCHAR traFilter[100];
	m_edtArtist.GetWindowText(artFilter, 100);
	m_edtTrack.GetWindowText(traFilter, 100);
	int CurDateSel = m_cmbDate.GetCurSel() + 1;


	eng->Execute(_T("DROP VIEW TopTracksHelper"));
	_tcsncpy(bf, _T("CREATE VIEW TopTracksHelper AS "), 2000);
	size_t curLen = _tcslen(bf);


	if (CurDateSel != TI_Disabled)
	{
		HistTracksViewSchemaHelper::GetSQL(&bf[curLen], 2000 - curLen, &m_MinTime, 
			m_CurrentDatePos ? &m_MaxTime : NULL, 
			artFilter, 
			traFilter, 1);
		eng->Execute(bf);
		HistTracksViewSchema::GetSQL(bf, 2000, &m_MinTime, 
			m_CurrentDatePos ? &m_MaxTime : NULL, 
			artFilter, 
			traFilter, 1, 0, Sorting, SortingDESC);
	}

	else
	{
		HistTracksViewSchemaHelper::GetSQL(&bf[curLen], 2000 - curLen, NULL, 
			m_CurrentDatePos ? &m_MaxTime : NULL, 
			artFilter, 
			traFilter, 1);
		eng->Execute(bf);
		HistTracksViewSchema::GetSQL(bf, 2000, NULL, NULL, artFilter, traFilter, 1, 0, Sorting, SortingDESC);
	}
	schema->QueryDatabase(eng, bf, FALSE);
	return retVal;

}


void CHistoryDlg::OnEnChangeEdtArtist()
{
	RefreshView();
}

void CHistoryDlg::RefreshView()
{
	m_bHiddenViewNeedsRefresh = TRUE;
	int curSel = m_cmbDate.GetCurSel() + 1;
	TCHAR DateDesc[100];
	if (curSel != TI_Disabled)
	{
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);
		INT Diff = TimeDiff(curTime, m_FirstDate, (TimeIntervals) curSel);
		if (Diff > 0)
		{
			m_sldDate.EnableWindow(TRUE);
			m_sldDate.SetRange(-Diff, 0, TRUE); 
			m_sldDate.SetPos(m_CurrentDatePos);
			memcpy(&m_MinTime, &curTime, sizeof(SYSTEMTIME));
			ChangeTime(m_MinTime, (TimeIntervals) curSel, m_CurrentDatePos, TRUE);
			memcpy(&m_MaxTime, &curTime, sizeof(SYSTEMTIME));
			ChangeTime(m_MaxTime, (TimeIntervals) curSel, m_CurrentDatePos + 1, TRUE);
			switch (curSel)
			{
			case TI_Year:
				_sntprintf(DateDesc, 100, _T("Year: %d"), curTime.wYear + m_CurrentDatePos);
				break;
			case TI_Month:
				{
					_tcsncpy(DateDesc, _T("Month: "), 100);
					int curLen = _tcslen(DateDesc);
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_MinTime, _T("MMMM, yyyy"), DateDesc, 100 - curLen);
				}
				break;
			case TI_Day:
				{
					_tcsncpy(DateDesc, _T("Day: "), 100);
					int curLen = _tcslen(DateDesc);
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_MinTime, _T("d MMMM, yyyy"), DateDesc, 100 - curLen);
				}
				break;
			case TI_Last365Days:
			case TI_Last30Days:
			case TI_Last10Days:
				{
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_MinTime, _T("d MMMM, yyyy"), DateDesc, 100);
					_tcsncat(DateDesc, _T(" until "), 100);
					int curLen = _tcslen(DateDesc);
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_MaxTime, _T("d MMMM, yyyy"), &DateDesc[curLen], 100 - curLen);
				}
				break;
			case TI_Last24Hours:
					_tcsncpy(DateDesc, _T("Last 24 Hours."), 100);
				break;
			default:
				ASSERT(FALSE);
			}

		}
		else
		{
			m_sldDate.EnableWindow(FALSE);
			_tcsncpy(DateDesc, _T("Not Enough Data."), 100);
		}
		
	}
	else
	{
		m_sldDate.EnableWindow(FALSE);
		m_CurrentDatePos = 0;
		_tcsncpy(DateDesc, _T("All Time."), 100);
	}
	SetDlgItemText(IDC_LBLDATE, DateDesc);
	int CurTab = m_Tab.GetCurSel() + 1;
	switch (CurTab)
	{
	case TAB_History:
		m_List.ShowWindow(SW_SHOW);
		m_ListTopTra.ShowWindow(SW_HIDE);
		m_ListTopArt.ShowWindow(SW_HIDE);
		m_List.Refresh();
		break;
	case TAB_TopTracks:
		m_List.ShowWindow(SW_HIDE);
		m_ListTopTra.ShowWindow(SW_SHOW);
		m_ListTopArt.ShowWindow(SW_HIDE);
		m_ListTopTra.Refresh();
		break;
	case TAB_TopArtists:
		m_List.ShowWindow(SW_HIDE);
		m_ListTopTra.ShowWindow(SW_HIDE);
		m_ListTopArt.ShowWindow(SW_SHOW);
		m_ListTopArt.Refresh();
		break;
	default:
		ASSERT(FALSE);
	}
}

INT CHistoryDlg::TimeDiff(SYSTEMTIME& first, SYSTEMTIME& last, TimeIntervals ti)
{
	INT ret = 0;
	switch (ti)
	{
	case TI_Year:
		ret = first.wYear - last.wYear;
		break;
	case TI_Month:
		ret = (first.wYear - last.wYear) * 12 + first.wMonth - last.wMonth;
		break;
	case TI_Day:
	case TI_Last365Days:
	case TI_Last30Days:
	case TI_Last10Days:
	case TI_Last24Hours:
		{
			DOUBLE f,l;
			SystemTimeToVariantTime(&first, &f);
			SystemTimeToVariantTime(&last, &l);
			switch (ti)
			{
			case TI_Day:
				ret = (INT) (f - l);
				break;
			case TI_Last365Days:
				ret = (INT) (f - l) / 365 + 1;
				break;
			case TI_Last30Days:
				ret = (INT) (f - l) / 30 + 1;
				break;
			case TI_Last10Days:
				ret = (INT) (f - l) / 10 + 1;
				break;
			case TI_Last24Hours:
				ret = (INT) (f - l) + 1;
				break;
			default:
				ASSERT(FALSE);
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}
	return ret;
}


void CHistoryDlg::ChangeTime(SYSTEMTIME& st, TimeIntervals ti, INT val, BOOL SetStart)
{
	switch (ti)
	{
	case TI_Year:
		st.wYear = st.wYear + val;
		if (st.wYear < 1910)
			st.wYear = 1910;
		if (SetStart)
		{
			st.wMonth = 1;
			st.wDay = 1;
			st.wHour = 0;
			st.wMinute = 0;
			st.wSecond = 0;
		}
		break;
	case TI_Month:
		{
			ChangeTime(st, TI_Year, val / 12);
			int nMonth = st.wMonth + val % 12;
			if (nMonth < 1)
			{
				ChangeTime(st, TI_Year, -1);
				st.wMonth = nMonth + 12;
			}
			else if (nMonth > 12)
			{
				ChangeTime(st, TI_Year, 1);
				st.wMonth = nMonth - 12;
			}
			else
				st.wMonth = nMonth;
			if (SetStart)
			{
				st.wDay = 1;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
			}
		}
		break;
	case TI_Day:
	case TI_Last365Days:
	case TI_Last30Days:
	case TI_Last10Days:
	case TI_Last24Hours:
		{
			DOUBLE vTime;
			SystemTimeToVariantTime(&st, &vTime);
			switch (ti)
			{
			case TI_Day:									break;
			case TI_Last365Days:	val = 365 * (val - 1);	break;
			case TI_Last30Days:		val = 30 * (val - 1);	break;
			case TI_Last10Days:		val = 10 * (val - 1);	break;
			case TI_Last24Hours:	val = val - 1;			break;
			}
			vTime += val;
			
			VariantTimeToSystemTime(vTime, &st);
			if (SetStart)
			{
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
			}
		}
		break;

	}

}

void CHistoryDlg::OnEnChangeEdtTrack()
{
	RefreshView();
}

void CHistoryDlg::OnCbnSelchangeCmbdate()
{
	m_CurrentDatePos = 0;
	RefreshView();
}

void CHistoryDlg::UpdateFirstDate()
{
	HistLogTableSchema hLog(ObjFactory::Instance()->GetDbEngine());
	if (!hLog.GetFirstDate(m_FirstDate))
		GetLocalTime(&m_FirstDate);
}


//void CHistoryDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	// TODO: Add your message handler code here and/or call default
//	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
//}

void CHistoryDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->m_hWnd == m_sldDate.m_hWnd)
	{
		TRACE("nSBCode: %d nPos: %d\r\n", nSBCode, nPos);
		m_CurrentDatePos = m_sldDate.GetPos();// - MaxPos;
		RefreshView();
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CHistoryDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	RefreshView();
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
