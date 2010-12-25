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
#include "StatusBarWnd.h"
#include "PrgAPI.h"
#include "ActionManager.h"
#include "MediaPlayer.h"
#include "InfoDownloadManager.h"
#include "CollectionManager.h"
#include "MenuManager.h"

//#define TIMER_MSGDELAY 100
#define TIMER_CHECKSTATUS 200
#define TIMER_CHECKSTATUS_DELAY 1000

#define IDC_INFOTEXT 200
#define IDC_MEDIAANIM 210
#define IDC_MEDIATEXT 220
#define IDC_NETANIM 230
#define IDC_NETTEXT 240



const int cMargins = 1;
const int cBtnWidth = 34;
const int cVolumeWidth = 70;
const int cEQSliderWidth = 8;
const int cLevelWidth = 8;
const int cSmallBtnWidth = 16;
const int cStatusTextHeight = 18;
const int cSliderHeight = 14;
const int cSeparator = 8;


CStatusBarWnd::CStatusBarWnd(CWnd* pParent /*=NULL*/)
	:
	m_rcInfo(0,0,0,0),
	m_netPendingJobs(0)
{
	//PRGAPI()->RegisterControl(this);

}

CStatusBarWnd::~CStatusBarWnd()
{
	//TRACEST(_T("CStatusBarChDlg::~CStatusBarChDlg"));
	//PrgAPI* pAPI = PRGAPI();
	//pAPI->UnRegisterControl(this);
}

BEGIN_MESSAGE_MAP(CStatusBarWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CStatusBarWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CWnd::Create(_T("STATIC"), NULL, dwStyle | SS_NOTIFY, rect, pParentWnd, nID, NULL) == FALSE)
		return FALSE;
	PrgAPI* pAPI = PRGAPI();
	CRect rc(0,0,0,0);
	m_ticker.Create(_T("STATIC"), NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, rc, this, IDC_INFOTEXT);
	m_ticker.AddMessage(CTickerWnd::MT_Permanent, CTeenSpiritApp::sAppTitle, 0);

	m_NetText.Create(_T("STATIC"), NULL, WS_CHILD, rc, this, IDC_NETTEXT);
	m_NetText.SetBorderType(CLabelEx::B_NoBorder, 4, 4);

	m_NetAnim.Create(_T("STATIC"), NULL, WS_CHILD | WS_VISIBLE, rc, this, IDC_NETANIM);

	m_MediaText.Create(_T("STATIC"), NULL, WS_CHILD, rc, this, IDC_MEDIATEXT);
	m_MediaText.SetBorderType(CLabelEx::B_NoBorder, 4, 4);

	m_MediaAnim.Create(_T("STATIC"), NULL, WS_CHILD, rc, this, IDC_MEDIAANIM);

	//SetColor(COL_Bk, RGB(10,10,10));
	//SetColor(COL_Text, RGB(200,200,220));
	SetTimer(TIMER_CHECKSTATUS, TIMER_CHECKSTATUS_DELAY, NULL);
	ApplyTranslation(*pAPI->GetTranslationManager());
	ApplySkin(pAPI->GetSkinManager()->GetSkin());
	return TRUE;
}


// CStatusBarChDlg message handlers

void CStatusBarWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CStatusBarWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	ResizeWindow(cx, cy);
}



void CStatusBarWnd::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;

	//=== Fonts
	CFont* pFont = skin.GetFont(CSEC_StatusBar, FSTY_Normal);
	m_ticker.SetFont(*pFont);
	m_NetText.SetFont(pFont);
	m_MediaText.SetFont(pFont);

	//=== Colors
	SetColor(COL_Bk, skin.GetColor(CSEC_StatusBar, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_StatusBar, CVAL_Text));

}



void CStatusBarWnd::ResizeWindow(int cx, int cy)
{
	if (cx > 0 && cy > 0 && m_NetAnim.m_hWnd)
	{
		m_rcInfo.SetRect(cMargins, cMargins, cx - cMargins, cy - cMargins);
		ResizeInfoControls();
	}
}

void CStatusBarWnd::ResizeInfoControls()
{
	ASSERT(m_NetText.m_hWnd != NULL);
	ASSERT(m_rcInfo.Width() > 0 && m_rcInfo.Height() > 0);
	INT curx = m_rcInfo.right;
	INT hei = m_rcInfo.Height();
	if (m_NetText.IsWindowVisible())
	{
		UINT len = m_NetText.CalculateWidth();
		curx -= len;
		m_NetText.MoveWindow(curx, m_rcInfo.top, len, hei);
		curx -= cMargins;
		curx -= hei;
		m_NetAnim.MoveWindow(curx, m_rcInfo.top, hei, hei);
		curx -= cMargins;
	}
	if (m_MediaText.IsWindowVisible())
	{
		UINT len = m_MediaText.CalculateWidth();
		curx -= len;
		m_MediaText.MoveWindow(curx, m_rcInfo.top, len, hei);
		curx -= cMargins;
		curx -= hei;
		m_MediaAnim.MoveWindow(curx, m_rcInfo.top, hei, hei);
		curx -= cMargins;
	}
	m_ticker.MoveWindow(m_rcInfo.left, m_rcInfo.top, curx - m_rcInfo.left, hei);
}



void CStatusBarWnd::ApplyTranslation(ITranslation& translation)
{
	PrgAPI* pAPI = PRGAPI();
	if (m_NetText.m_hWnd)
	{
		m_NetText.SetWindowText(pAPI->GetString(IDS_DOWNLOADING));
	}
	Invalidate(TRUE);
}

void CStatusBarWnd::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	//case TIMER_MSGDELAY:
	//	KillTimer(nIDEvent);
	//	m_InfoText.SetIcon(NULL, 0 ,0);
	//	m_InfoText.SetWindowText(_T(""));
	//	break;
	case TIMER_CHECKSTATUS:
		Update();
		break;
	default:
		ASSERT(0);
		__super::OnTimer(nIDEvent);
		break;
	}
}

void CStatusBarWnd::Show(LPCTSTR msg, LPCTSTR title, SEVERITY sev)
{
	CTickerWnd::MESSAGE_TYPE mt = CTickerWnd::MT_Normal;
	if (sev == SEV_InfoL2 || sev==SEV_InfoL3)
		mt = CTickerWnd::MT_Optional;

	APP_ICONS icon = ICO_Info;
	if (sev == SEV_Warning)
		icon = ICO_Warning;
	else if (sev == SEV_Error)
		icon = ICO_Error;
	m_ticker.AddMessage(mt, msg, PRGAPI()->GetIcon(icon));
}

void CStatusBarWnd::Update()
{
	PrgAPI* pAPI = PRGAPI();
	if (pAPI->InfoDownloadManagerExists())
	{
		InfoDownloadManager* pIDM = pAPI->GetInfoDownloadManager();
		INT netPendingJobs = pIDM->GetPendingJobs();
		if (netPendingJobs != m_netPendingJobs)
		{
			if (m_netPendingJobs == 0)
			{
				//===It was hidden. We must show it
				if (!m_NetAnim.HasPicture())
				{
					//===Initialize Net Anim if needed
					m_NetAnim.LoadResourceID(IDR_PROGRESSANI, _T("PNG"));
					m_NetAnim.SetFrameSize(22,22);
				}
				//m_NetAnim.SetBkColor(RGB(255,0,0),255);

				m_NetAnim.Start();
				m_NetText.ShowWindow(SW_SHOW);
				m_NetAnim.ShowWindow(SW_SHOW);
			}
			if (netPendingJobs == 0)
			{
				//===It was visible. Now we must hide it
				m_NetText.ShowWindow(SW_HIDE);
				m_NetAnim.ShowWindow(SW_HIDE);
				ResizeInfoControls();
				m_NetAnim.Stop();
			}
			else
			{
				//===It was visible. Now we must update it
				TCHAR bf[500];
				_sntprintf(bf, 500, _T("%s [%d]"), pAPI->GetString(IDS_DOWNLOADING), netPendingJobs);
				m_NetText.SetWindowText(bf);
				ResizeInfoControls();
			}
			m_netPendingJobs = netPendingJobs;
		}
	}
	if (pAPI->CollectionManagerExists())
	{
		CollectionManager* pCM = pAPI->GetCollectionManager();
		const CollectionUpdateJob::Progress& progress = pCM->GetProgress();
		if (progress.curAction != m_oldColProgress.curAction || 
			progress.curPos != m_oldColProgress.curPos)
		{
			if (m_oldColProgress.curAction == CollectionUpdateJob::ST_Idle)
			{
				//===It was hidden. We must show it
				if (!m_MediaAnim.HasPicture())
				{
					//===Initialize Media Anim if needed
					m_MediaAnim.LoadResourceID(IDR_PROGRESSANI, _T("PNG"));
					m_MediaAnim.SetFrameSize(22,22);
				}
				m_MediaAnim.Start();
				m_MediaAnim.ShowWindow(SW_SHOW);
				m_MediaText.ShowWindow(SW_SHOW);
			}
			if (progress.curAction == CollectionUpdateJob::AC_Idle)
			{
				//===It was visible. Now we must hide it
				m_MediaText.ShowWindow(SW_HIDE);
				m_MediaAnim.ShowWindow(SW_HIDE);
				ResizeInfoControls();
				m_MediaAnim.Stop();
			}
			else
			{
				TCHAR bf[200];
				BOOL bShowInfo = TRUE;
				switch (progress.curAction)
				{
				case CollectionUpdateJob::AC_RemovingInexistentFiles:
					_sntprintf(bf, 200, _T("%s [%d]"), 
						pAPI->GetString(IDS_REMOVINGFILES), 
						pCM->GetPendingJobs());
					break;
				case CollectionUpdateJob::AC_CountingDirectoryFiles:
					_sntprintf(bf, 200, _T("%s [%d]"), 
						pAPI->GetString(IDS_COUNTINGFILES), 
						pCM->GetPendingJobs());
					break;
				case CollectionUpdateJob::AC_AddingUpdatingFiles:
					if (progress.maxPos > 0)
						_sntprintf(bf, 200, _T("%s %d/%d [%d]"), 
							pAPI->GetString(IDS_SCANNINGFILES), 
							progress.curPos, 
							progress.maxPos,
							pCM->GetPendingJobs());
					else
						_sntprintf(bf, 200, _T("%s %d [%d]"), 
							pAPI->GetString(IDS_SCANNINGFILES), 
							progress.curPos, 
							pCM->GetPendingJobs());
					break;
				default:
					bShowInfo = FALSE;
					break;
				}
				if (bShowInfo)
				{
					m_MediaText.SetWindowText(bf);
					ResizeInfoControls();
				}
			}
			m_oldColProgress = progress;
		}
	}
}

BOOL CStatusBarWnd::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_LBUTTONDBLCLK && pMsg->hwnd == m_ticker.m_hWnd)
		PRGAPI()->GetActionManager()->ShowUsersLog();
	return __super::PreTranslateMessage(pMsg);
}

BOOL CStatusBarWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CStatusBarChDlg::OnCommand"), wParam);
	return 0;
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CStatusBarWnd::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (idx == COL_Bk)
		{
			m_ticker.SetColor(CTickerWnd::COL_BkNorm, value);
			m_ticker.SetColor(CTickerWnd::COL_BkOpti, value);
			m_ticker.SetColor(CTickerWnd::COL_BkPerm, value);
			m_MediaText.SetColor(CLabelEx::COL_Bk, value);
			m_NetText.SetColor(CLabelEx::COL_Bk, value);
			m_NetAnim.SetBkColor(value);
			m_MediaAnim.SetBkColor(value);
		}
		else if (idx == COL_Text)
		{
			m_ticker.SetColor(CTickerWnd::COL_TextNorm, value);
			m_ticker.SetColor(CTickerWnd::COL_TextOpti, value);
			m_ticker.SetColor(CTickerWnd::COL_TextPerm, value);
			m_MediaText.SetColor(CLabelEx::COL_Text, value);
			m_NetText.SetColor(CLabelEx::COL_Text, value);
			m_MediaText.SetColor(CLabelEx::COL_Fore, value);
			m_NetText.SetColor(CLabelEx::COL_Fore, value);
		}
		Invalidate(TRUE);
	}
}
COLORREF CStatusBarWnd::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
//const TCHAR* const cColorNames[] = {
//	_T("Bk"),
//	_T("Text"),
//	_T("Slider1"),
//	_T("Slider1"),
//	NULL
//};
//LPCTSTR CStatusBarWnd::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
BOOL CStatusBarWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, GetColor(COL_Bk));
	return TRUE;//__super::OnEraseBkgnd(pDC);
}
