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
#include "TSBrowserEnh.h"
#include "PrgAPI.h"
#include "ActionManager.h"

#define ID_FIRSTBUTTON	200
#define ID_BROWSER		300
#define ID_URL			310
#define ID_PROGRESS		320


BEGIN_MESSAGE_MAP(TSBrowserEnh, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

TSBrowserEnh::TSBrowserEnh():
m_bInitialized(FALSE)
{
	m_colors[COL_Bk] = GetSysColor(COLOR_WINDOW);
	m_colors[COL_Text] = GetSysColor(COLOR_MENUTEXT);
}
TSBrowserEnh::~TSBrowserEnh()
{

}

void TSBrowserEnh::SetURL(LPCTSTR url)
{
	if (m_Address.m_hWnd)
		m_Address.SetWindowText(url);
	if (m_Browser.m_hWnd)
		m_Browser.Navigate(url);
}

APP_ICONS btn2IcoId[] = {
	ICO_Back16,
	ICO_Forward16,
	ICO_Refresh16,
	ICO_Home16

};

void TSBrowserEnh::OnHome()
{
	if (m_Browser.m_hWnd)
	{
		m_Browser.Clear();
		m_Browser.Write(_T("<html><body bgcolor=\"#000000\"></body></html>"));
	}
	if (m_Address.m_hWnd)
		m_Address.SetWindowText(_T(""));
}

void TSBrowserEnh::OnInit()
{
	CRect rc(0,0,0,0);
	PrgAPI* pAPI = PRGAPI();
	for (int i=0; i<BTN_Last; i++)
	{
		m_Buttons[i].Create(_T(""), WS_VISIBLE | WS_CHILD, rc, this, ID_FIRSTBUTTON + i);
		m_Buttons[i].ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
		m_Buttons[i].SetComponents(FlexButton::COMP_Image);
		m_Buttons[i].SetIcon(pAPI->GetIcon(btn2IcoId[i]), 16, 16);
		m_Buttons[i].SetFrameDrawEffect(FlexButton::ST_Disabled, FlexButton::FDE_None);

		//m_Buttons[i].SetColor(FlexButton::COL_Bk, m_colors[COL_Bk]);
		////btn.setGradient(true);

	}
	m_Address.Create(WS_VISIBLE | WS_CHILD | ES_MULTILINE, rc, this, ID_URL);
	m_Address.SetIcon(pAPI->GetIcon(ICO_Internet16));
	//m_Address.SetColor(CIconEditEx::COL_Bk, m_colors[COL_Text]);//Reverse
	//m_Address.SetColor(CIconEditEx::COL_Text, m_colors[COL_Bk]);//Reverse
	m_Browser.SetNotifier(this);
	if (m_Browser.Create(WS_VISIBLE | WS_CHILD, rc, this, ID_BROWSER))
		OnHome();
	ApplySkin(pAPI->GetSkinManager()->GetSkin());
}

const int cBarHeight = 20;

void TSBrowserEnh::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	if (m_hWnd != NULL && cx>0 && cy>0)
	{
		if (m_bInitialized == FALSE)
		{
			OnInit();
			m_bInitialized = TRUE;
		}
		const int cMargin = 1;
		int curX = cMargin;
		int curY = cMargin;//cy - cBarHeight - 2 * cMargin;
		for (int i = 0; i < BTN_Last; i++)
		{
			m_Buttons[i].MoveWindow(curX, curY, cBarHeight, cBarHeight, FALSE);
			curX += cBarHeight + cMargin;
		}
		int wid = cx - curX - cMargin;
		m_Address.MoveWindow(curX, curY, wid, cBarHeight, FALSE);
		curY += cBarHeight + 1;
		m_Browser.MoveWindow(0, curY, cx, cy - curY, TRUE);
	}

}

void TSBrowserEnh::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	SetColor(COL_Bk, skin.GetColor(CSEC_InfoControl, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_InfoControl, CVAL_Text));
	m_Address.SetFont(skin.GetFont(CSEC_Global, FSTY_Normal));
	for (int i=0; i<BTN_Last; i++)
	{
		m_Buttons[i].SetColor(FlexButton::COL_Bk, m_colors[COL_Bk]);
	}
	m_Address.SetColor(CIconEditEx::COL_Bk, m_colors[COL_Text]);//Reverse
	m_Address.SetColor(CIconEditEx::COL_Text, m_colors[COL_Bk]);//Reverse
}




BOOL TSBrowserEnh::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	CRect browserRect;
	m_Browser.GetWindowRect(&browserRect);
	ScreenToClient(&browserRect);
	if (browserRect.top == 0)
		rc.top = browserRect.bottom;
	else
		rc.bottom = browserRect.top;
	pDC->FillSolidRect(&rc, m_colors[COL_Bk]);
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}


//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void TSBrowserEnh::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (idx == COL_Bk)
		{
			for (int i = 0; i<BTN_Last; i++)
				m_Buttons[i].SetColor(FlexButton::COL_Bk, value);
			m_Address.SetColor(CIconEditEx::COL_Text, value);//Reverse
		}
		else if (idx == COL_Text)
		{	
			m_Address.SetColor(CIconEditEx::COL_Bk, value);//Reverse
		}
		if (m_hWnd)	
			Invalidate();
	}
}
COLORREF TSBrowserEnh::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
//const TCHAR* const cColorNames[] = {
//	_T("Bk"),
//	_T("Text"),
//	NULL
//};
//LPCTSTR TSBrowserEnh::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------

BOOL TSBrowserEnh::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("TSBrowserEnh::OnCommand"), wParam);
	switch (wParam)
	{
	//case BTN_Add + ID_FIRSTBUTTON:
	//	//MessageBox( m_Browser.GetLocationURL(), m_Browser.GetLocationName(), MB_OK);
	//	{
	//		TagInfo ti;
	//		ti.Title = m_Browser.GetLocationName();
	//		PRGAPI()->GetActionManager()->AddVirtualTrackDialog(m_Browser.GetLocationURL(), ti);
	//	}
	//	break;
	case BTN_Home + ID_FIRSTBUTTON:
		OnHome();
		break;
	case BTN_Back + ID_FIRSTBUTTON:
		m_Browser.GoBack();
		break;
	case BTN_Forward + ID_FIRSTBUTTON:
		m_Browser.GoForward();
		break;
	case BTN_Refresh + ID_FIRSTBUTTON:
		m_Browser.Refresh();
		break;
	}
	return __super::OnCommand(wParam, lParam);
}


BOOL TSBrowserEnh::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP && pMsg->hwnd == m_Address.m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString url;
		m_Address.GetWindowText(url);
		SetURL(url);
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void TSBrowserEnh::OnProgressChange(long progress,long progress_max)
{
	if (m_Address.m_hWnd)
	{
		DOUBLE prog = 0.0;
		if (progress_max > 0 && progress > 0)
			prog = progress / (DOUBLE)progress_max;
		m_Address.SetProgressValue(prog);
	}
}
void TSBrowserEnh::OnNavigateComplete(LPCTSTR url)
{
	if (m_Address.m_hWnd)
	{
		if (_tcsicmp(url, _T("about:blank")) != 0)
			m_Address.SetWindowText(url);
	}
}
void TSBrowserEnh::EnableBackButton(BOOL bEnable)
{
	if (m_Buttons[BTN_Back].m_hWnd)
		m_Buttons[BTN_Back].EnableWindow(bEnable);

}
void TSBrowserEnh::EnableNextButton(BOOL bEnable)
{
	if (m_Buttons[BTN_Forward].m_hWnd)
		m_Buttons[BTN_Forward].EnableWindow(bEnable);
}

