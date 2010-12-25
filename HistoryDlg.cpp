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
#include "PrgAPI.h"
#include "SQLManager.h"
#include "AppSettings/AppSettings.h"
#include "cStringUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define ID_REFRESH_TIMER 100
#define ID_REFRESH_TIMER_DELAY 500

IMPLEMENT_DYNAMIC(CHistoryDlg, CDialog)

CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistoryDlg::IDD, pParent),
	m_CurrentDatePos(0),
	m_bRefreshDisabled(FALSE)
{
	m_hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_HISTORY32), IMAGE_ICON, 16, 16, LR_VGACOLOR);//LR_LOADTRANSPARENT LR_VGACOLOR
	//m_List[TAB_History] = new CHistMainListCtrl();
	//m_List[TAB_TopTracks] = new CHistTracksListCtrl();
	//m_List[TAB_TopArtists] = new CHistArtistsListCtrl();
}

CHistoryDlg::~CHistoryDlg()
{
	DestroyIcon(m_hIcon);
	//for (int i = 0; i < TAB_Last; i++)
	//	delete m_List[i];
	if (m_hWnd) DestroyWindow();

}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_LIST, m_HMainList);
	DDX_Control(pDX, IDC_LISTTOPTRA, m_HTracksList);
	DDX_Control(pDX, IDC_LISTTOPART, m_HArtistsList);
	DDX_Control(pDX, IDC_EDT_TRACK, m_edtTrack);
	DDX_Control(pDX, IDC_EDT_ARTIST, m_edtArtist);
	DDX_Control(pDX, IDC_CMBDATE, m_cmbDate);
	DDX_Control(pDX, IDC_SLIDER, m_sldDate);
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
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CHistoryDlg::OnBnClickedBtnReset()
{
	m_bRefreshDisabled = TRUE;
	BOOL bSomethingChanged = FALSE;
	if (m_cmbDate.GetCurSel() != 0)
	{
		bSomethingChanged = TRUE;
		m_cmbDate.SetCurSel(0);
	}
	if (m_edtArtist.GetWindowTextLength() != 0)
	{
		bSomethingChanged = TRUE;
		m_edtArtist.SetWindowText(_T(""));
	}
	if (m_edtTrack.GetWindowTextLength() != 0)
	{
		bSomethingChanged = TRUE;
		m_edtTrack.SetWindowText(_T(""));
	}
	m_bRefreshDisabled = FALSE;
	if (bSomethingChanged)
		RefreshView();
}
static const UINT indicators[] =
{
	ID_SEARCH_STAT_MSG,
	ID_SEARCH_STAT_RECORDS,
	ID_SEARCH_STAT_TIME
};


LPCTSTR cCHistoryDlgSection = _T("HistoryDlg");

BOOL CHistoryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitTab();
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	m_cmbDate.AddString(pAPI->GetString(IDS_ALLTIME));
	m_cmbDate.AddString(pAPI->GetString(IDS_YEARS));
	m_cmbDate.AddString(pAPI->GetString(IDS_MONTHS));
	m_cmbDate.AddString(pAPI->GetString(IDS_DAYS));
	TCHAR bf[100];
	_sntprintf(bf, 100, _T("%s: 365"), pAPI->GetString(IDS_DAYS));
	m_cmbDate.AddString(bf);
	_sntprintf(bf, 100, _T("%s: 30"), pAPI->GetString(IDS_DAYS));
	m_cmbDate.AddString(bf);
	_sntprintf(bf, 100, _T("%s: 10"), pAPI->GetString(IDS_DAYS));
	m_cmbDate.AddString(bf);
	m_cmbDate.AddString(pAPI->GetString(IDS_24HOURS));
	m_cmbDate.SetCurSel(0);
	//GetFirstDate


	SQLManager::HistoryLogStats stats;
	if (pSM->GetHistoryLogStats(stats))
		m_FirstDate = stats.firstTime;
	else
		GetLocalTime(&m_FirstDate);



	m_HMainList.Init(&m_dlgFilter);
	m_HTracksList.Init(&m_dlgFilter);
	m_HArtistsList.Init(&m_dlgFilter);

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	VERIFY(m_StatusBar.CreateEx(this, SBARS_SIZEGRIP, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM) != 0);
	m_StatusBar.SetBarStyle(m_StatusBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_StatusBar.SetIndicators(indicators, 3);
	m_StatusBar.SetPaneInfo(0, ID_SEARCH_STAT_MSG, SBPS_STRETCH | SBPS_POPOUT, 200);
	m_StatusBar.SetPaneInfo(1, ID_SEARCH_STAT_RECORDS, SBPS_NORMAL, 200);
	m_StatusBar.SetPaneInfo(2, ID_SEARCH_STAT_TIME, SBPS_NORMAL, 200);
	CRect rc;
	GetClientRect(&rc);
	OnSize(0, rc.Width(), rc.Height());
	m_Tab.SetCurSel(TAB_History);

	//RefreshView();
	LRESULT a = 0;
	OnTcnSelchangeTab(NULL, &a);
	m_HArtistsList.SetHistoryDlg(this);
	m_HTracksList.SetHistoryDlg(this);
	m_HMainList.SetFocus();

	SetDlgItemText(IDC_TIMEINTERVAL, pAPI->GetString(IDS_TIMEINTERVAL));
	//SetDlgItemText(IDC_PAST, pLang->GetString(IDS_PAST));
	SetDlgItemText(IDC_TIMESLIDER, pAPI->GetString(IDS_TIMESLIDER));
	//SetDlgItemText(IDC_NOW, pLang->GetString(IDS_NOW));
	SetDlgItemText(IDC_ACTIVE, pAPI->GetString(IDS_ACTIVE));
	SetDlgItemText(IDC_TEXTFILTERS, pAPI->GetString(IDS_TEXTFILTERS));
	SetDlgItemText(IDC_TITLE, pAPI->GetString(IDS_TITLE));
	SetDlgItemText(IDC_ARTIST, pAPI->GetString(IDS_ARTIST));
	SetDlgItemText(IDC_DATEFILTER, pAPI->GetString(IDS_DATEFILTER));
	SetWindowText(pAPI->GetString(IDS_HISTORY));
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) pAPI->GetFont(FONT_Dialogs)->m_hObject);

	AppSettings* params = pAPI->GetAppSettings();
	WINDOWPLACEMENT pl;
	GetWindowPlacement(&pl);
	int showCmd;
	params->Read(cCHistoryDlgSection, _T("CMDSHOW"), showCmd, SW_SHOWNORMAL);
	params->Read(cCHistoryDlgSection, _T("LEFT"), (int&) pl.rcNormalPosition.left, 40);
	params->Read(cCHistoryDlgSection, _T("TOP"), (int&) pl.rcNormalPosition.top, 25);
	params->Read(cCHistoryDlgSection, _T("RIGHT"), (int&) pl.rcNormalPosition.right, 900);
	params->Read(cCHistoryDlgSection, _T("BOTTOM"), (int&) pl.rcNormalPosition.bottom, 650);
	//pl.showCmd = SW_SHOWMINIMIZED;
	pl.showCmd = showCmd;
	ModifyStyle(WS_VISIBLE, 0);
	SetWindowPlacement(&pl);

	return TRUE;
}


void CHistoryDlg::SetArtistFilterText(LPCTSTR artist, BOOL bAutoSwitch /*= FALSE*/)
{
	ASSERT(artist != NULL);
	if (bAutoSwitch)
		m_Tab.SetCurSel(0);
	SetDlgItemText(IDC_EDT_ARTIST, artist);
	SetDlgItemText(IDC_EDT_TRACK, NULL);
	RefreshView();
}

void CHistoryDlg::SetTrackFilterText(LPCTSTR track, LPCTSTR artist, BOOL bAutoSwitch /*= FALSE*/)
{
	ASSERT(track != NULL);
	ASSERT(artist != NULL);
	if (bAutoSwitch)
		m_Tab.SetCurSel(0);
	SetDlgItemText(IDC_EDT_TRACK, track);
	SetDlgItemText(IDC_EDT_ARTIST, artist);
	RefreshView();
}


void CHistoryDlg::InitTab()
{
	TCITEM tabItem;
	memset(&tabItem, 0, sizeof(TCITEM));
	tabItem.mask = TCIF_TEXT | TCIF_PARAM;
	tabItem.cchTextMax = 50;
	PrgAPI* pLang = PRGAPI();
	tabItem.pszText = (LPTSTR) pLang->GetString(IDS_RECORDEDHISTORY);
	m_Tab.InsertItem(TAB_History, &tabItem);
	tabItem.pszText = (LPTSTR) pLang->GetString(IDS_TOPSONGS);
	m_Tab.InsertItem(TAB_TopTracks, &tabItem);
	tabItem.pszText = (LPTSTR) pLang->GetString(IDS_TOPARTISTS);
	m_Tab.InsertItem(TAB_TopArtists, &tabItem);
}

void CHistoryDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rc;
	const int marginX = 5;
	const int marginY = 5;
	const int DistFromLeft = 200;
	const int TabHeight = 20;
	const int StatusBarHeight = 20; 
	if (IsWindow(m_Tab.m_hWnd))
	{
		m_Tab.SetWindowPos(&wndBottom,	
			DistFromLeft + marginX,		
			marginY,
			cx - DistFromLeft - 2 * marginX,
			cy - 2 * marginY - StatusBarHeight,
			NULL);
		INT lleft = DistFromLeft + 2 * marginX;
		INT ltop = TabHeight + 2 * marginY;
		INT lcx = cx - DistFromLeft - 4 * marginX;
		INT lcy = cy - TabHeight - 4 * marginY - StatusBarHeight;
		//for (int i = 0; i < TAB_Last; i++)
		//	m_List[i]->SetWindowPos(NULL, lleft, ltop, lcx, lcy, SWP_NOZORDER);
		m_HMainList.SetWindowPos(NULL, lleft, ltop, lcx, lcy, SWP_NOZORDER);
		m_HTracksList.SetWindowPos(NULL, lleft, ltop, lcx, lcy, SWP_NOZORDER);
		m_HArtistsList.SetWindowPos(NULL, lleft, ltop, lcx, lcy, SWP_NOZORDER);
		m_StatusBar.SetWindowPos(NULL, 0, cy - StatusBarHeight, cx, StatusBarHeight, SWP_NOZORDER);
	}
}

void CHistoryDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(630,430);
	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CHistoryDlg::UpdateStatusBar(DWORD LastTickCount, UINT RecordCount)
{
	TCHAR* msg = NULL;
	TCITEM pItem;
	memset(&pItem, 0, sizeof(TCITEM));
	TCHAR bf[2000];
	pItem.mask = TCIF_TEXT;
	pItem.pszText = bf;
	pItem.cchTextMax = 2000;
	m_Tab.GetItem(m_Tab.GetCurSel(), &pItem);
	m_StatusBar.SetPaneText(0, pItem.pszText);

	//::LoadString(AfxGetResourceHandle(), m_StatusBar.GetItemID(1), bf, 2000);
	PrgAPI* pLang = PRGAPI();
	_sntprintf(bf, 2000, _T("%s: %d"), pLang->GetString(IDS_RECORDS), RecordCount);
	m_StatusBar.SetPaneText(1, bf);

	//::LoadString(AfxGetResourceHandle(), m_StatusBar.GetItemID(2), bf, 2000);
	_sntprintf(bf, 2000, _T("%s: %d ms"), pLang->GetString(IDS_PROCESSTIME), GetTickCount() - LastTickCount);
	m_StatusBar.SetPaneText(2, bf);
}


void CHistoryDlg::OnEnChangeEdtArtist()
{
	DelayedRefreshView();
}

void CHistoryDlg::RefreshView()
{
	if (m_bRefreshDisabled)
		return;
	DWORD ticks = GetTickCount();
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
			memcpy(&m_dlgFilter.MinTime, &curTime, sizeof(SYSTEMTIME));
			ChangeTime(m_dlgFilter.MinTime, (TimeIntervals) curSel, m_CurrentDatePos, TRUE);
			memcpy(&m_dlgFilter.MaxTime, &curTime, sizeof(SYSTEMTIME));
			ChangeTime(m_dlgFilter.MaxTime, (TimeIntervals) curSel, m_CurrentDatePos + 1, TRUE);
			PrgAPI* pLang = PRGAPI();
			switch (curSel)
			{
			case TI_Year:
				_sntprintf(DateDesc, 100, _T("%s: %d"), pLang->GetString(IDS_YEAR), curTime.wYear + m_CurrentDatePos);
				break;
			case TI_Month:
				{
					int curLen = _sntprintf(DateDesc, 100, _T("%s: "), pLang->GetString(IDS_MONTH));
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_dlgFilter.MinTime, _T("MMM, yy"), DateDesc, 100 - curLen);
				}
				break;
			case TI_Day:
				{
					int curLen = _sntprintf(DateDesc, 100, _T("%s: "), pLang->GetString(IDS_DAY));
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_dlgFilter.MinTime, _T("d MMM, yy"), DateDesc, 100 - curLen);
				}
				break;
			case TI_Last365Days:
			case TI_Last30Days:
			case TI_Last10Days:
			case TI_Last24Hours:
				{
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_dlgFilter.MinTime, _T("d MMM, yy "), DateDesc, 100);
					//_tcsncat(DateDesc, _T(" until "), 100);
					_tcscat(DateDesc, pLang->GetString(IDS_UNTILL));
					int curLen = _tcslen(DateDesc);
					GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &m_dlgFilter.MaxTime, _T(" d MMM, yy"), &DateDesc[curLen], 100 - curLen);
				}
				break;
			default:
				ASSERT(FALSE);
			}
			m_dlgFilter.MinTimeStatus = HistoryDlgFilter::FS_Enabled;
			if (m_CurrentDatePos == 0)
				m_dlgFilter.MaxTimeStatus = HistoryDlgFilter::FS_Disabled;
			else
				m_dlgFilter.MaxTimeStatus = HistoryDlgFilter::FS_Enabled;
		}
		else
		{
			m_sldDate.EnableWindow(FALSE);
			_tcsncpy(DateDesc, PRGAPI()->GetString(IDS_NOTENOUGHDATA), 100);
		}
		
	}
	else
	{
		m_sldDate.EnableWindow(FALSE);
		m_CurrentDatePos = 0;
		_tcsncpy(DateDesc, PRGAPI()->GetString(IDS_ALLTIME), 100);
		m_dlgFilter.MaxTimeStatus = HistoryDlgFilter::FS_Disabled;
		m_dlgFilter.MinTimeStatus = HistoryDlgFilter::FS_Disabled;
	}
	SetDlgItemText(IDC_LBLDATE, DateDesc);
	m_edtTrack.GetWindowText(m_dlgFilter.Track, 100);
	m_edtArtist.GetWindowText(m_dlgFilter.Artist, 100);
	m_dlgFilter.TrackStatus = m_dlgFilter.Track[0] == 0 ? HistoryDlgFilter::FS_Disabled : HistoryDlgFilter::FS_Enabled;
	m_dlgFilter.ArtistStatus = m_dlgFilter.Artist[0] == 0 ? HistoryDlgFilter::FS_Disabled : HistoryDlgFilter::FS_Enabled;
	int CurTab = m_Tab.GetCurSel();
	switch (CurTab)
	{
	case 0:
		m_HMainList.Refresh();
		UpdateStatusBar(ticks, m_HMainList.GetRecordCount());
		break;
	case 1:
		m_HTracksList.Refresh();
		UpdateStatusBar(ticks, m_HTracksList.GetRecordCount());
		break;
	case 2:
		m_HArtistsList.Refresh();
		UpdateStatusBar(ticks, m_HArtistsList.GetRecordCount());
		break;

	}
	m_HMainList.ShowWindow((CurTab == 0) ? SW_SHOW : SW_HIDE);
	m_HTracksList.ShowWindow((CurTab == 1) ? SW_SHOW : SW_HIDE);
	m_HArtistsList.ShowWindow((CurTab == 2) ? SW_SHOW : SW_HIDE);
	//m_List[m_Tab.GetCurSel()]->Refresh();
	//UpdateStatusBar(ticks, m_List[m_Tab.GetCurSel()]->GetRecordCount());
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
	DelayedRefreshView();
}

void CHistoryDlg::OnCbnSelchangeCmbdate()
{
	m_CurrentDatePos = 0;
	DelayedRefreshView();
}

void CHistoryDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->m_hWnd == m_sldDate.m_hWnd)
	{
		TRACE(_T("nSBCode: %d nPos: %d\r\n"), nSBCode, nPos);
		INT newPos = m_sldDate.GetPos();
		if (newPos != m_CurrentDatePos)
		{
			m_CurrentDatePos = newPos;
			DelayedRefreshView();
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CHistoryDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	RefreshView();
	//int CurTab = m_Tab.GetCurSel();
	//m_HMainList.ShowWindow((CurTab == 0) ? SW_SHOW : SW_HIDE);
	//m_HTracksList.ShowWindow((CurTab == 1) ? SW_SHOW : SW_HIDE);
	//m_HArtistsList.ShowWindow((CurTab == 2) ? SW_SHOW : SW_HIDE);

	//for (int i = 0; i < TAB_Last; i++)
	//{
	//	if (i == CurTab)
	//		m_List[i]->ShowWindow(SW_SHOW);
	//	else
	//		m_List[i]->ShowWindow(SW_HIDE);
	//}
	*pResult = 0;
}

void CHistoryDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case ID_REFRESH_TIMER:
		RefreshView();
		KillTimer(nIDEvent);
		break;
	}
}

void CHistoryDlg::DelayedRefreshView()
{
	KillTimer(ID_REFRESH_TIMER);
	SetTimer(ID_REFRESH_TIMER, ID_REFRESH_TIMER_DELAY, NULL);
}

BOOL CHistoryDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		//Disable closing the window by pressing return
		return FALSE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CHistoryDlg::OnDestroy()
{

	PrgAPI* pAPI = PRGAPI();
	AppSettings* params = pAPI->GetAppSettings();
	WINDOWPLACEMENT pl;
	GetWindowPlacement(&pl);
	params->Write(cCHistoryDlgSection, _T("CMDSHOW"), (int&) pl.showCmd);
	params->Write(cCHistoryDlgSection, _T("LEFT"), (int&) pl.rcNormalPosition.left);
	params->Write(cCHistoryDlgSection, _T("TOP"), (int&) pl.rcNormalPosition.top);
	params->Write(cCHistoryDlgSection, _T("RIGHT"), (int&) pl.rcNormalPosition.right);
	params->Write(cCHistoryDlgSection, _T("BOTTOM"), (int&) pl.rcNormalPosition.bottom);
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}
