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
#include "SearchDlg.h"

#include "AppSettings/AppSettings.h"
#include "TracksFilter.h"

#include "PrgApi.h"

#include "cStringUtils.h"
#include "stlStringUtils.h"
#include "ActionManager.h"
#include "SQLManager.h"

// CSearchDlg dialog


//#define ID_REFRESH_TIMER 100
//#define ID_REFRESH_TIMER_DELAY 500

IMPLEMENT_DYNAMIC(CSearchDlg, CDialog)

CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDlg::IDD, pParent),
	m_bInitialized(FALSE)
{
	m_hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_SEARCH32), IMAGE_ICON, 16, 16, LR_VGACOLOR);//LR_LOADTRANSPARENT LR_VGACOLOR
	//m_SearchLst.SetIdentifier(_T("SRCLIST"));
}

CSearchDlg::~CSearchDlg()
{
	if (m_hWnd) DestroyWindow();
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LST_SEARCH, m_SearchLst);
	//DDX_Control(pDX, IDC_LISTBOX, m_FullTextListCtrl);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	ON_MESSAGE(WM_PAGEUPDATED, &CSearchDlg::OnPageUpdated)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CSearchDlg::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		TCITEM tabItem;
		tabItem.mask = TCIF_TEXT;
		tabItem.cchTextMax = 200;
		int curItem = 0;
		CTabCtrl* tab = m_Sheet.GetTabControl();
		tabItem.pszText = (LPTSTR) pLang->GetString(IDS_SEARCH);
		tab->SetItem(curItem++, &tabItem);
		tabItem.pszText = (LPTSTR) pLang->GetString(IDS_ADVSEARCH);
		tab->SetItem(curItem++, &tabItem);
		tabItem.pszText = (LPTSTR) pLang->GetString(IDS_LATESTADDITIONSTITLE);
		tab->SetItem(curItem++, &tabItem);
		tabItem.pszText = (LPTSTR) pLang->GetString(IDS_RATING);
		tab->SetItem(curItem++, &tabItem);
		tabItem.pszText = (LPTSTR) pLang->GetString(IDS_FULLTEXTTITLE);
		tab->SetItem(curItem++, &tabItem);


		if (m_ListBoxMenu.m_hMenu)
			m_ListBoxMenu.DestroyMenu();
		m_ListBoxMenu.CreatePopupMenu();
		m_ListBoxMenu.SetFont(pLang->GetFont(FONT_Dialogs));
		m_ListBoxMenu.AppendMenu(MF_STRING, ACMD_Play, (LPTSTR) pLang->GetString(IDS_PLAY));
		m_ListBoxMenu.SetDefaultItem(ACMD_Play);
		m_ListBoxMenu.AppendMenu(MF_STRING, ACMD_Enqueue, (LPTSTR) pLang->GetString(IDS_ENQUEUE));
		m_ListBoxMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		m_ListBoxMenu.AppendMenu(MF_STRING, ACMD_OpenFolder, (LPTSTR) pLang->GetString(IDS_OPENCONTAININGFOLDER));
		
		m_FullTextListCtrl.SetMessageOnEmpty(pLang->GetString(IDS_FULLTEXTTEXT));
		if (m_hWnd)
		{
			RefreshMainList();
			Invalidate();
		}
	}
}


static const UINT indicators[] =
{
	ID_SEARCH_STAT_MSG,
	ID_SEARCH_STAT_RECORDS,
	ID_SEARCH_STAT_TIME

};
BOOL CSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_SearchLst.Create(WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), this, 345);
	LoadOptions();

	VERIFY(m_StatusBar.CreateEx(this, SBARS_SIZEGRIP, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM) != 0);
	m_StatusBar.SetBarStyle(m_StatusBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_StatusBar.SetIndicators(indicators, 3);
	m_StatusBar.SetPaneInfo(0, ID_SEARCH_STAT_MSG, SBPS_STRETCH | SBPS_POPOUT, 200);
	m_StatusBar.SetPaneInfo(1, ID_SEARCH_STAT_RECORDS, SBPS_NORMAL, 200);
	m_StatusBar.SetPaneInfo(2, ID_SEARCH_STAT_TIME, SBPS_NORMAL, 200);

	m_SimpleSearchPP.SetFileTypeOption(CSimpleSearchCHdlg::FTO_All);
	m_SimpleSearchPP.SetRealParent(this);
	m_Sheet.AddPage(&m_SimpleSearchPP);

	m_DetailedSearchPP.SetRealParent(this);
	m_Sheet.AddPage(&m_DetailedSearchPP);

	SYSTEMTIME st;
	GetLocalTime(&st);
	DOUBLE curTime;
	SystemTimeToVariantTime(&st, &curTime);
	curTime -= 10;
	VariantTimeToSystemTime(curTime, &st);
	m_LatestAdditionsPP.SetFirstDate(&st);
	m_LatestAdditionsPP.SetRealParent(this);
	m_Sheet.AddPage(&m_LatestAdditionsPP);

	m_RatingPP.SetRatingOption(100, TRUE);
	m_RatingPP.SetRealParent(this);
	m_Sheet.AddPage(&m_RatingPP);

	//m_SpecialInfoPP.SetComments(FALSE);
	//m_SpecialInfoPP.SetLyrics(TRUE);
	//m_SpecialInfoPP.SetPersonal(FALSE);
	//m_SpecialInfoPP.SetRealParent(this);
	//m_Sheet.AddPage(&m_SpecialInfoPP);

	PrgAPI* pAPI = PRGAPI();
	m_FullTextListCtrl.Create(WS_VISIBLE | WS_CHILDWINDOW | WS_BORDER, CRect(), this, 654);

	m_FullTextPP.SetTextTypeOption(CFullTextSearchCHdlg::TT_Lyrics);
	m_FullTextPP.SetRealParent(this);
	m_Sheet.AddPage(&m_FullTextPP);

	m_Sheet.EnableStackedTabs(FALSE);
	m_Sheet.Create(this, WS_CHILD | WS_VISIBLE | WS_TABSTOP);//, WS_EX_CONTROLPARENT);
	m_Sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);

	m_FullTextListCtrl.GetScrollBar().SetIcons(pAPI->GetIcon(ICO_Up16), pAPI->GetIcon(ICO_Down16), 16, 16);


	CRect rc;
	GetClientRect(&rc);
	OnSize(NULL, rc.Width(), rc.Height());
	ApplyTranslation(*pAPI->GetTranslationManager());
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);

	m_bInitialized = TRUE;

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchDlg::LoadOptions()
{
	LoadState(PRGAPI()->GetAppSettings());
}

void CSearchDlg::SaveOptions()
{
	SaveState(PRGAPI()->GetAppSettings());
}

void CSearchDlg::SetSimpleSearchText(LPCTSTR searchString)
{
	if (m_hWnd != NULL)
	{
		if (m_Sheet.GetActiveIndex() != 0)
			m_Sheet.SetActivePage(0);
		m_SimpleSearchPP.SetSimpleSearchText(searchString);
	}
}

LRESULT CSearchDlg::OnPageUpdated(WPARAM wParam, LPARAM lParam)
{
	if (m_bInitialized)
		RefreshMainList();
	return 0;
}

void CSearchDlg::RefreshMainList()
{
	TracksFilter tf;
	BOOL retVal = TRUE;
	DWORD tick = GetTickCount();
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	m_StatusBar.SetPaneText(0, pAPI->GetString(IDS_PLEASEWAIT));


	TabID curTab = (TabID)(m_Sheet.GetActiveIndex() + 1);
	if (curTab == TAB_FullText)
	{
		m_FullTextListCtrl.ShowWindow(SW_SHOW);
		m_SearchLst.ShowWindow(SW_HIDE);
	}
	else
	{
		m_FullTextListCtrl.ShowWindow(SW_HIDE);
		m_SearchLst.ShowWindow(SW_SHOW);
	}

	UINT itemCount = 0;

	FullTrackRecordCollection& trackList = m_SearchLst.GetFullTrackRecordCollection();
	switch (curTab)
	{
		//void SetFilter(UINT artID = 0, UINT albID = 0, UINT GetTCHAR* SimpleSearch = NULL)
	case TAB_SimpleSearch://1
		{
			switch (m_SimpleSearchPP.GetFileTypeOption())
			{
			case CSimpleSearchCHdlg::FTO_Audio:
				tf.TrackType.match = NUMM_Under;
				tf.TrackType.val = 100;
				break;
			case CSimpleSearchCHdlg::FTO_Video:
				tf.TrackType.match = NUMM_Over;
				tf.TrackType.val = 99;
				break;
			case CSimpleSearchCHdlg::FTO_All:
				break;
			default:
				ASSERT(0);//??
				break;
			}
			TCHAR bf[100];
			m_SimpleSearchPP.GetSimpleSearchText(bf, 100);
			bf[99] = 0;
			trackList.clear();
			if (bf[0] != 0)
				pSM->GetFullTrackRecordCollectionBySmartSearch(trackList, bf);
			//m_SearchLst.Update();
		}
		break;
	case TAB_DetailedSearch://1
		{
			LPCTSTR val = m_DetailedSearchPP.GetArtist();
			val = m_DetailedSearchPP.GetArtist();
			BOOL bQueryIsEmpty = TRUE;
			if (val[0] != 0)
			{
				tf.Artist.match=TXTM_Like;
				tf.Artist.val = val;
				bQueryIsEmpty = FALSE;
			}
			val = m_DetailedSearchPP.GetAlbum();
			if (val[0] != 0)
			{
				tf.Album.match=TXTM_Like;
				tf.Album.val = val;
				bQueryIsEmpty = FALSE;
			}
			val = m_DetailedSearchPP.GetTitle();
			if (val[0] != 0)
			{
				tf.Title.match=TXTM_Like;
				tf.Title.val = val;
				bQueryIsEmpty = FALSE;
			}
			val = m_DetailedSearchPP.GetLocation();
			if (val[0] != 0)
			{
				tf.Location.match=TXTM_Like;
				tf.Location.val = val;
				bQueryIsEmpty = FALSE;
			}
			trackList.clear();
			if (bQueryIsEmpty == FALSE)
				pSM->GetFullTrackRecordCollectionByTracksFilter(trackList, tf);
			//m_SearchLst.Update();
		}
		break;
	case TAB_LatestTracks://2
		{
			SYSTEMTIME st;
			m_LatestAdditionsPP.GetFirstDate(&st);
			tf.DateAdded.match = DATM_After;
			tf.DateAdded.val = st;
			trackList.clear();
			pSM->GetFullTrackRecordCollectionByTracksFilter(trackList, tf);
			//m_SearchLst.Update();
		}
		break;
	case TAB_BestWorstTracks://3
		{
			INT rating;
			BOOL bover;
			m_RatingPP.GetRatingOption(rating, bover);
			tf.Rating.match = bover ? NUMM_Over : NUMM_Under;
			tf.Rating.val = rating;//CTeenSpiritApp::Stars2Rating(rating + 1);;
			trackList.clear();
			pSM->GetFullTrackRecordCollectionByTracksFilter(trackList, tf);
			//m_SearchLst.Update();
		}
		break;
	case TAB_FullText:
		{
			TCHAR Search[100];
			m_FullTextPP.GetSearchText(Search, 100);
			Search[99] = 0;
			UINT iconID = 0;
			if (Search[0] != 0)
			{	
				InfoItemTypeEnum st;
				switch(m_FullTextPP.GetTextTypeOption())
				{
				case CFullTextSearchCHdlg::TT_Lyrics:
					st = IIT_TrackLyrics;
					iconID = 10;
					break;
				case CFullTextSearchCHdlg::TT_Comments:
					st = IIT_TrackComment;
					iconID = 6;
					break;
				case CFullTextSearchCHdlg::TT_Personal:
					st = IIT_TrackPersonal;
					iconID = 8;
					break;
				default:
					ASSERT(0);
					break;
				}
				m_FullTextListCtrl.SetFilter(Search, st);
				itemCount = m_FullTextListCtrl.GetItemCount();
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}
	if (curTab != TAB_FullText)
	{
		m_SearchLst.Update();
		itemCount = m_SearchLst.GetItemCount();
	}

	//TCHAR* msg = NULL;
	TCHAR bf[100];
	TCITEM tabItem;
	tabItem.cchTextMax = 100;
	tabItem.mask = TCIF_TEXT;
	tabItem.pszText = bf;

	m_Sheet.GetTabControl()->GetItem(m_Sheet.GetActiveIndex(), &tabItem);
	//m_Sheet.GetPage(it)->GetWindowText(bf, 100);
	m_StatusBar.SetPaneText(0, bf);

	_sntprintf(bf, 100, _T("%s: %d"), pAPI->GetString(IDS_TRACKS), itemCount);
	//::LoadString(AfxGetResourceHandle(), m_StatusBar.GetItemID(1), bf, 2000);
	//size_t curLen = _tcslen(bf);
	//_sntprintf(&bf[curLen], 2000 - curLen, _T("%d"), itemCount);
	m_StatusBar.SetPaneText(1, bf);

	_sntprintf(bf, 100, _T("%s: %d ms"), pAPI->GetString(IDS_PROCESSTIME), GetTickCount() - tick);

	//::LoadString(AfxGetResourceHandle(), m_StatusBar.GetItemID(2), bf, 2000);
	//curLen = _tcslen(bf);
	//_sntprintf(&bf[curLen], 2000 - curLen, _T("%d ms"), GetTickCount() - tick);
	m_StatusBar.SetPaneText(2, bf);

	//	size_t curLen = _tcslen(bf);
	//	_sntprintf(&bf[curLen], 2000 - curLen, _T(". %d Records. Job done in %dms."), schema->GetRecordCount(), GetTickCount() - tick);
	//	m_StatusBar.SetWindowText(bf);
//	m_SearchLst.SetFilter(tf);

}


void CSearchDlg::OnSize(UINT nType, int cx, int cy)
{
	const int marginX = 5;
	const int marginY = 5;
	const int TabControlHeight = 150;
	const int StatusBarHeight = 20;
	if (m_SearchLst.m_hWnd != 0)
	{
		m_Sheet.SetWindowPos(NULL, marginX, marginY, cx - 2 * marginX, TabControlHeight, SWP_NOZORDER);
		m_SearchLst.SetWindowPos(NULL, marginX, 2 * marginY + TabControlHeight, cx - 2 * marginX, cy - (2 * marginY + TabControlHeight) - marginY - StatusBarHeight, SWP_NOZORDER);
		m_StatusBar.SetWindowPos(NULL, 0, cy - StatusBarHeight, cx, StatusBarHeight, SWP_NOZORDER);
		m_FullTextListCtrl.SetWindowPos(NULL, marginX, 2 * marginY + TabControlHeight, cx - 2 * marginX, cy - (2 * marginY + TabControlHeight) - marginY - StatusBarHeight, SWP_NOZORDER);
	}
}

void CSearchDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(700, 250);
}

void CSearchDlg::OnTcnSelchangeTabSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	RefreshMainList();
	*pResult = 0;
}

void CSearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


void CSearchDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

BOOL CSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			OnCancel();
			return TRUE;
		case VK_RETURN:
			return TRUE;
		}

	}
	if (pMsg->message == WM_LBUTTONDBLCLK)
	{
		if (pMsg->hwnd == m_FullTextListCtrl.m_hWnd)
			PlayListBoxTrack(FALSE);
	}
	return __super::PreTranslateMessage(pMsg);
}

//void CSearchDlg::CreateImageList()
//{
//	BOOL	bRetValue = FALSE;
//	HICON	hIcon = NULL;
//
//	CBitmap		cBitmap;
//	BITMAP		bmBitmap;
//
//	if (!cBitmap.Attach(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_LVICONS),
//		IMAGE_BITMAP, 0, 0,
//		LR_DEFAULTSIZE|LR_CREATEDIBSECTION)) ||
//		!cBitmap.GetBitmap(&bmBitmap))
//		return;
//
//	CSize		cSize(bmBitmap.bmWidth, bmBitmap.bmHeight); 
//	RGBTRIPLE*	rgb		= (RGBTRIPLE*)(bmBitmap.bmBits);
//	//COLORREF rgbMask = RGB(255,0,255);
//	if (m_ImageList.Create(16, 16, ILC_COLOR24|ILC_MASK, 12, 0))
//	{
//		if (m_ImageList.Add(&cBitmap, RGB(255,0,255)) == -1)
//			return;
//	}
//	else
//		TRACE(_T("@0CSearchDlg::CreateImageList. Failed"));
// 
//	cBitmap.Detach();
//	return ;
//}

void CSearchDlg::PlayListBoxTrack(BOOL bEnqueue)
{
	FullTrackRecordSP ft = m_FullTextListCtrl.GetFullTrackRecord(m_FullTextListCtrl.GetNextSelectedItem());
	if (ft.get() != NULL)
	{
		FullTrackRecordCollection col;
		col.push_back(ft);
		PrgAPI* pAPI = PRGAPI();
		ActionManager* pAM = pAPI->GetActionManager();
		if (bEnqueue)
			pAM->Enqueue(pAPI->GetMediaPlayer(), col);
		else
			pAM->Play(pAPI->GetMediaPlayer(), col);

	}
}

BOOL CSearchDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CSearchDlg::OnCommand"), wParam);
	switch (wParam)
	{
	case ACMD_Play:
		PlayListBoxTrack(FALSE);
		break;
	case ACMD_Enqueue:
		PlayListBoxTrack(TRUE);
		break;
	case ACMD_OpenFolder:
		{
			FullTrackRecordSP ft = m_FullTextListCtrl.GetFullTrackRecord(m_FullTextListCtrl.GetNextSelectedItem());
			if (ft.get() != NULL)
			{
				std::tstring param = _T("/select,");
				param += ft->track.location;
				ShellExecute(NULL, NULL, _T("explorer.exe"), param.c_str(), NULL, SW_SHOWNORMAL);
			}
		}
		break;
	default:
		return CDialog::OnCommand(wParam, lParam);
	//default:
	//	ASSERT(FALSE);
	}
	return TRUE;
}

void CSearchDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd->m_hWnd == m_FullTextListCtrl.m_hWnd)
	{
		CPoint p = point;
		m_FullTextListCtrl.ScreenToClient(&p);
		int item = m_FullTextListCtrl.GetItemFromPoint(p.x, p.y);
		if (item >=0)
		{
			m_FullTextListCtrl.SetItemSelection(item, TRUE);
			m_ListBoxMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, point.x, point.y, this);

		}
	}
}

const LPCTSTR cSearchDlgSection = _T("SearchDlg");
const LPCTSTR cSearchDlgSortingCount = _T("SortingCount");


BOOL CSearchDlg::LoadState(AppSettings* pAppSettings)
{
	CMainListCtrl& list = m_SearchLst;


	INT value = 0;
	//=== Read Column Visibility / Width / Position Info
	TCHAR bf[500];
	CMainListCtrl::SColumnInfo* colInfo = list.GetColumnInfo();
	for (INT i = CMainListCtrl::CL_First; i < CMainListCtrl::CL_Last; i++)
	{
		_sntprintf(bf, 500, _T("ColInfo_%02d_vis"), i);
		pAppSettings->Read(cSearchDlgSection, bf, value, 1);
		colInfo[i].bVisible =  value;//GetIntSetting(bf) > 0;
		_sntprintf(bf, 500, _T("ColInfo_%02d_order"), i);
		pAppSettings->Read(cSearchDlgSection, bf, value, i);
		if (value >= 0 && value < 100)
			colInfo[i].iOrder = value;
		_sntprintf(bf, 500, _T("ColInfo_%02d_cx"), i);
		pAppSettings->Read(cSearchDlgSection, bf, value, 0);
		if (value >= 0 && value < 10000)
			colInfo[i].cx = value;
		if (colInfo[i].cx < 1)
			colInfo[i].cx = 1;
	}
	list.UpdateColumns();


	//=== Read the sorting information
	SortOptionCollection& soc = list.GetSortingInfo();
	INT sortCount = 0;
	pAppSettings->Read(cSearchDlgSection, cSearchDlgSortingCount, sortCount, 1);
	for (INT i = 0; i < sortCount; i++)
	{
		SortOption so(RSO_None, TRUE);
		INT sortOption = 0;
		_sntprintf(bf, 500, _T("Sorting_%02d_option"), i);
		pAppSettings->Read(cSearchDlgSection, bf, value, RSO_None);
		if (value >= RSO_None && value < RSO_Last)
			so.option = RecordSortOptionsEnum(value);

		_sntprintf(bf, 500, _T("Sorting_%02d_asc"), i);
		pAppSettings->Read(cSearchDlgSection, bf, value, 0);
		so.ascending = value != 0;

		soc.ApplySortOption(so);
	}
	if (sortCount > 0)
		list.UpdateSorting();
	return TRUE;
}
BOOL CSearchDlg::SaveState(AppSettings* pAppSettings)				
{
	CMainListCtrl& list = m_SearchLst;

	//=== Write Column Visibility / Width / Position Info
	TCHAR bf[500];
	CMainListCtrl::SColumnInfo* colInfo = list.GetColumnInfo();
	for (int i = 0; i < CMainListCtrl::CL_Last; i++)
	{
		_sntprintf(bf, 500, _T("ColInfo_%02d_vis"), i);
		pAppSettings->Write(cSearchDlgSection, bf, (INT)colInfo[i].bVisible);
		_sntprintf(bf, 500, _T("ColInfo_%02d_order"), i);
		pAppSettings->Write(cSearchDlgSection, bf, (INT)colInfo[i].iOrder);
		_sntprintf(bf, 500, _T("ColInfo_%02d_cx"), i);
		pAppSettings->Write(cSearchDlgSection, bf, (INT)colInfo[i].cx);
	}

	//=== Write the sorting information
	SortOptionCollection& soc = list.GetSortingInfo();
	pAppSettings->Write(cSearchDlgSection, cSearchDlgSortingCount, soc.GetSortOptionsCount());
	for (UINT i =0; i< soc.GetSortOptionsCount(); i++)
	{
		SortOption so = soc.GetSortOption(i);
		_sntprintf(bf, 500, _T("Sorting_%02d_option"), i);
		pAppSettings->Write(cSearchDlgSection, bf, so.option);
		_sntprintf(bf, 500, _T("Sorting_%02d_asc"), i);
		pAppSettings->Write(cSearchDlgSection, bf, so.ascending);
	}
	return TRUE;
}

void CSearchDlg::OnDestroy()
{
	SaveOptions();
	__super::OnDestroy();

	// TODO: Add your message handler code here
}
