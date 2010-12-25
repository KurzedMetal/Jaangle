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
#include "TSPaneContainer.h"
#include "PrgAPI.h"
#include "PaneController.h"
#include "TSMenu.h"

#define TIMER_CAPTIONROTATION 100
#define TIMER_CAPTIONROTATION_DELAY 3000

#define LEFT_BUTTONS_STARTID	10
#define RIGHT_BUTTONS_STARTID	50

#define CMD_ADDPANEPOSITION 0x750
//#define CMD_SWITCHPANE 0x800

enum SystemButtons
{
	SB_First = RIGHT_BUTTONS_STARTID,
	SB_AddPane,
	SB_Close,
	SB_MaxRestore,
	SB_Last
};


BEGIN_MESSAGE_MAP(TSPaneContainer, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCDESTROY()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

TSPaneContainer::TSPaneContainer(PaneController *pPaneController):
m_rotationDelay(3000),
m_captionPos(0),
m_pContainedPane(NULL),
m_captionHeight(18),
m_bPassedOnEraseBk(FALSE),
m_pPaneController(pPaneController),
m_pPosistioningMenu(NULL),
m_bIsMaximized(FALSE)
{
}
TSPaneContainer::~TSPaneContainer()
{
	SetContainedPane(NULL);
	if (m_hWnd)
		DestroyWindow();
	ButtonCollection::iterator it = m_userButtons.begin();
	for (;it!=m_userButtons.end();it++)
		delete *it;
	it = m_systemButtons.begin();
	for (;it!=m_systemButtons.end();it++)
	{
		delete *it;
	}
}


//BOOL TSPaneContainer::Show()
//{
//	if (m_hWnd)
//	{
//		if (m_pContainedPane)
//			m_pContainedPane->Show();
//		ShowWindow(SW_SHOW);
//		SetTimer(TIMER_CAPTIONROTATION, m_rotationDelay, NULL);
//		return TRUE;
//	}
//	return FALSE;
//}
//void TSPaneContainer::Hide()
//{
//	if (m_hWnd)
//	{
//		if (m_pContainedPane)
//			m_pContainedPane->Hide();
//		ShowWindow(SW_HIDE);
//		KillTimer(TIMER_CAPTIONROTATION);
//	}
//}

//void TSPaneContainer::SaveSettings()
//{
//	if (m_pContainedPane == NULL)
//		m_pContainedPane->SaveSettings();
//}


//=== ExtraInterface

FlexButton* TSPaneContainer::FindButton(INT nID, ButtonCollection& btnCollection) 
{
	ButtonCollection::iterator it = btnCollection.begin();
	FlexButton* btn = NULL;
	for (; it != btnCollection.end(); it++)
	{
		if ((*it)->GetDlgCtrlID() == nID)
			return *it;
	}
	return NULL;
}

BOOL TSPaneContainer::AddUserButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon)
{
	FlexButton* btn = FindButton(nID, m_userButtons);
	if (btn != NULL)
		return FALSE;
	btn = new FlexButton;
	btn->Create(_T(""), WS_CHILDWINDOW|WS_VISIBLE, CRect(), this, nID);
	SetButton(*btn, text, hIcon, cxIcon, cyIcon);
	m_userButtons.push_back(btn);
	return TRUE;
}

BOOL TSPaneContainer::SetUserButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon)
{
	FlexButton* btn = FindButton(nID, m_userButtons);
	if (btn == NULL)
		return FALSE;
	SetButton(*btn, text, hIcon, cxIcon, cyIcon);
	return TRUE;
}

BOOL TSPaneContainer::AddSystemButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon)
{
	FlexButton* btn = FindButton(nID, m_systemButtons);
	if (btn != NULL)
		return FALSE;

	btn = new FlexButton;
	btn->Create(_T(""), WS_CHILDWINDOW|WS_VISIBLE, CRect(), this, nID);
	SetButton(*btn, text, hIcon, cxIcon, cyIcon);
	m_systemButtons.push_back(btn);
	return TRUE;
}

BOOL TSPaneContainer::SetSystemButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon)
{
	FlexButton* btn = FindButton(nID, m_systemButtons);
	if (btn == NULL)
		return FALSE;
	SetButton(*btn, text, hIcon, cxIcon, cyIcon);
	return TRUE;
}

void TSPaneContainer::SetButton(FlexButton& btn, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon)
{
	if (text == NULL)
	{
		btn.SetComponents(FlexButton::COMP_Image);
	}
	else
	{
		btn.SetComponents(FlexButton::COMP_Image | FlexButton::COMP_Text);
		btn.SetFont(GetFont());
		btn.SetWindowText(text);
		if (hIcon != NULL)
			btn.SetTextStyle(FlexButton::TS_SingleLine);
		else
			btn.SetTextStyle(FlexButton::TS_SingleLineCenter);
	}
	btn.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	if (hIcon != NULL)
		btn.SetIcon(hIcon, cxIcon, cyIcon);

}

BOOL TSPaneContainer::RemoveSystemButton(INT nID)
{
	ButtonCollection::iterator it = m_userButtons.begin();
	FlexButton* btn = NULL;
	for (; it != m_systemButtons.end(); it++)
	{
		if ((*it)->GetDlgCtrlID() == nID)
		{
			btn = *it;
			m_systemButtons.erase(it);
			break;
		}
	}
	if (btn == NULL)
		return FALSE;
	btn->DestroyWindow();
	delete btn;
	return TRUE;
}




void TSPaneContainer::RemoveButtons(BOOL bLeft)
{
	if (bLeft)
		m_userButtons.clear();
	else
		m_systemButtons.clear();
}

void TSPaneContainer::SetContainedPane(IPane* pPane)
{
	if (m_pContainedPane != NULL)
	{
		m_pContainedPane->UnInit();
		delete m_pContainedPane;
	}
	m_pContainedPane = pPane;
	if (m_hWnd && pPane != NULL)
	{
		pPane->Init(GetSafeHwnd());
		PaneButtonInfo bInfo;
		UINT i = 0;
		while (pPane->GetButtonInfo(bInfo, i))
		{
			AddUserButton(LEFT_BUTTONS_STARTID + i, bInfo.text, bInfo.hIcon, bInfo.iconSize, bInfo.iconSize);
			i++;
		}
	}
}

void TSPaneContainer::ApplyTranslation(ITranslation& translation)
{
	if (m_pContainedPane != NULL && m_hWnd != NULL)
	{
		PaneButtonInfo bInfo;
		UINT i = 0;
		while (m_pContainedPane->GetButtonInfo(bInfo, i))
		{
			SetUserButton(LEFT_BUTTONS_STARTID + i, bInfo.text, bInfo.hIcon, bInfo.iconSize, bInfo.iconSize);
			i++;
		}
		CRect rc;
		GetClientRect(&rc);
		PositionControls(rc.Width(), rc.Height());
		Invalidate(TRUE);	
	}
}




//void TSPaneContainer::Start()
//{
//	ASSERT(GetSafeHwnd() != NULL);
//	if (m_pContainedPane != NULL)
//		m_pContainedPane->Show();
//	if (GetSafeHwnd() != NULL)
//		SetTimer(TIMER_CAPTIONROTATION, TIMER_CAPTIONROTATION_DELAY, NULL);
//}


void TSPaneContainer::SetCaptionHeight(UINT height)
{
	m_captionHeight = height;
	if (m_hWnd != NULL)
	{
		CRect rc;
		GetClientRect(&rc);
		PositionControls(rc.Width(), rc.Height());
	}
}

void TSPaneContainer::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	switch (idx)
	{
	case COL_Bk:
		{
			ButtonCollection::iterator it = m_userButtons.begin();
			for (;it!=m_userButtons.end();it++)
			{
				(*it)->SetColor(FlexButton::COL_Bk, value);
				(*it)->SetColor(FlexButton::COL_Over, value);
				(*it)->SetColor(FlexButton::COL_Down, value);
			}
			it = m_systemButtons.begin();
			for (;it!=m_systemButtons.end();it++)
			{
				(*it)->SetColor(FlexButton::COL_Bk, value);
				(*it)->SetColor(FlexButton::COL_Over, value);
				(*it)->SetColor(FlexButton::COL_Down, value);
			}
		}
		break;
	case COL_Text:
		{
			ButtonCollection::iterator it = m_userButtons.begin();
			for (;it!=m_userButtons.end();it++)
			{
				(*it)->SetColor(FlexButton::COL_Text, value);
				(*it)->SetColor(FlexButton::COL_DownText, value);
				(*it)->SetColor(FlexButton::COL_OverText, value);
			}
			it = m_systemButtons.begin();
			for (;it!=m_systemButtons.end();it++)
			{
				(*it)->SetColor(FlexButton::COL_Text, value);
				(*it)->SetColor(FlexButton::COL_DownText, value);
				(*it)->SetColor(FlexButton::COL_OverText, value);
			}
			break;		

		}
	}
	if (m_hWnd)		
		Invalidate();
}
COLORREF TSPaneContainer::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

BOOL TSPaneContainer::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	if (m_bPassedOnEraseBk == FALSE)
	{
		m_bPassedOnEraseBk = TRUE;
		PositionControls(rc.Width(), rc.Height());
	}
	if (m_pContainedPane != NULL)
		rc.bottom = m_captionHeight;
	pDC->FillSolidRect(0,0,rc.Width(),rc.Height(), m_colors[COL_Bk]);
	return TRUE;//__super::OnEraseBkgnd(pDC);
}

void TSPaneContainer::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawPaneCaption(dc);
}

void TSPaneContainer::DrawPaneCaption(CDC& dc)
{
	CBrush bkBrush(m_colors[COL_CaptionBk]);
	//CPen outlinePen(PS_SOLID, 1, m_colors[COL_CaptionOutline]);
	CBrush* oldBrush = dc.SelectObject(&bkBrush);
	CPen* oldPen = (CPen*)dc.SelectStockObject(DC_PEN);
	CFont* oldFont = dc.SelectObject(GetFont());// dc.SelectObject(PRGAPI()->GetFont(FONT_Small));// (CFont*) dc.SelectStockObject(DEFAULT_GUI_FONT);

	CRect rc(m_captionRect);
	rc.DeflateRect(2,1);
	dc.SetDCPenColor(m_colors[COL_CaptionOutline]);
	dc.RoundRect(&rc, CPoint(5,5));

	dc.SetTextColor(m_colors[COL_CaptionText]);
	dc.SetBkMode(TRANSPARENT);
	rc.left += 3;
	rc.top += 1;
	rc.bottom -= 1;
	if (m_pContainedPane != NULL)
	{
		dc.DrawText(m_pContainedPane->GetTitle(m_captionPos), -1, &rc, DT_END_ELLIPSIS|DT_MODIFYSTRING|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
	dc.SelectObject(oldPen);
	dc.SelectObject(oldFont);
	dc.SelectObject(oldBrush);
}

BOOL TSPaneContainer::Create(CWnd* pParent, INT nID)
{
	if (GetSafeHwnd() == NULL)
	{
		if (CWnd::Create(_T("STATIC"), _T(""), WS_VISIBLE|WS_CHILDWINDOW|SS_NOTIFY, CRect(), pParent, nID))
		{
			PrgAPI* pAPI = PRGAPI();
			if (m_pContainedPane != NULL)
				SetContainedPane(m_pContainedPane);
			AddSystemButton(SB_Close, NULL, pAPI->GetIcon(ICO_Close16),16,16);
			AddSystemButton(SB_MaxRestore, NULL, pAPI->GetIcon(ICO_Maximize16),16,16);
			//AddButton(SB_Switch, _T("Switch"), 0,0,0,FALSE);
			AddSystemButton(SB_AddPane, NULL, pAPI->GetIcon(ICO_Add16),16,16);
			SetTimer(TIMER_CAPTIONROTATION, TIMER_CAPTIONROTATION_DELAY, NULL);
			return TRUE;
		}
	}
	return FALSE;
}


void TSPaneContainer::EnableCloseButton(BOOL bEnable)
{
	CWnd* pCloseButton = GetDlgItem(SB_Close);
	if (pCloseButton != NULL)
		pCloseButton->ShowWindow(bEnable ? SW_SHOW : SW_HIDE);
}

BOOL TSPaneContainer::IsCloseButtonEnabled()
{
	CWnd* pCloseButton = GetDlgItem(SB_Close);
	if (pCloseButton != NULL)
		return pCloseButton->IsWindowVisible();
	return FALSE;
}

void TSPaneContainer::SetMaximizedState(BOOL bMaximized)
{
	m_bIsMaximized = bMaximized;
	if (m_bIsMaximized)
	{
		FlexButton* btn = FindButton(SB_AddPane, m_systemButtons);
		if (btn != NULL)
			btn->ShowWindow(SW_HIDE);
		btn = FindButton(SB_Close, m_systemButtons);
		if (btn != NULL)
			btn->ShowWindow(SW_HIDE);
		btn = FindButton(SB_MaxRestore, m_systemButtons);
		if (btn != NULL)
			SetButton(*btn, NULL, PRGAPI()->GetIcon(ICO_Restore16), 16, 16);
	}
	else
	{
		FlexButton* btn = FindButton(SB_AddPane, m_systemButtons);
		if (btn != NULL)
			btn->ShowWindow(SW_SHOW);
		btn = FindButton(SB_Close, m_systemButtons);
		if (btn != NULL)
			btn->ShowWindow(SW_SHOW);
		btn = FindButton(SB_MaxRestore, m_systemButtons);
		if (btn != NULL)
			SetButton(*btn, NULL, PRGAPI()->GetIcon(ICO_Maximize16), 16, 16);

	}
}
BOOL TSPaneContainer::IsMaximizedState()
{
	return m_bIsMaximized;
}




void TSPaneContainer::ShowMenu(ITSMenu* pMenu, INT nButtonID)
{
	if (pMenu == NULL)
		return;
	CWnd* pButton = GetDlgItem(nButtonID);
	if (pButton != NULL)
	{
		CRect rc;
		pButton->GetWindowRect(&rc);
		pMenu->TrackPopupMenu(rc.left + (rc.Width() / 2), rc.bottom, this->GetSafeHwnd());
	}
}

BOOL TSPaneContainer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("TSPaneContainer::OnCommand wParam: %d\r\n"), wParam);
	if (wParam >= LEFT_BUTTONS_STARTID && wParam < m_userButtons.size() + LEFT_BUTTONS_STARTID)
	{
		//HMENU hMenu = m_pContainedPane->GetMenu(wParam - LEFT_BUTTONS_STARTID);
		ITSMenu* pMenu = m_pContainedPane->GetMenu(wParam - LEFT_BUTTONS_STARTID);
		if (pMenu != NULL)
		{
			ShowMenu(pMenu, wParam);
		}
		else
		{
			return m_pContainedPane->OnButton(wParam - LEFT_BUTTONS_STARTID);
		}
		return FALSE;
	}
	else if (wParam == SB_AddPane)
	{
		if (m_bIsMaximized == FALSE)
			m_pPaneController->AddPane(*this);
	}
	else if (wParam == SB_Close)
	{
		if (m_bIsMaximized == FALSE)
			m_pPaneController->ClosePane(*this);
	}
	else if (wParam == SB_MaxRestore)
	{
		m_pPaneController->MaxRestorePane(*this);
	}
	else
	{
		if (m_pContainedPane != NULL)
		{
			//=== Probably it is a menu item
			return m_pContainedPane->OnButton(wParam);
		}
	}
	return TRUE;

	//switch (wParam)
	//{
	//case SB_Close:
	//	break;


	//if (m_pPaneController)
	//	if (m_pPaneController->OnCommand(this, wParam))
	//		return TRUE;
	//if (m_pContainedPane)
	//	if (m_pContainedPane->OnControlButtonCommand(wParam))
	//		return TRUE;
	//return FALSE;
}

void TSPaneContainer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (cx > 0 && cy > 0)
	{
		if (m_bPassedOnEraseBk)
			PositionControls(cx, cy);
		else if (m_pContainedPane != NULL)
			m_pContainedPane->MovePane(0, m_captionHeight, cx, cy - m_captionHeight);
	}
}

void TSPaneContainer::PositionControls(int cx, int cy)
{
	if (cx == 0 || cy == 0)
		return;

	const INT cMargin = 1;
	INT curx = cMargin;
	ButtonCollection::iterator it = m_userButtons.begin();
	INT btnWidth = 20;//m_captionHeight - 2 * cMargin;//(*it)->CalcMinWidth(0);
	INT btnHeight = m_captionHeight - 2 * cMargin;
	for (;it!=m_userButtons.end();it++)
	{
		btnWidth = (*it)->CalcMinWidth() + 2 * cMargin;
		(*it)->MoveWindow(curx, cMargin, btnWidth, btnHeight);
		curx += btnWidth + cMargin;
	}
	m_captionRect = CRect(curx, cMargin, cx, m_captionHeight - cMargin);

	it = m_systemButtons.begin();
	curx = cx - cMargin;
	for (; it != m_systemButtons.end(); it++)
	{
		//INT wid = m_captionHeight - 2 * cMargin;//(*it)->CalcMinWidth(0);
		FlexButton* pBtn = *it;
		if (pBtn->IsWindowVisible())
		{
			btnWidth = pBtn->CalcMinWidth() + 2 * cMargin;
			curx -= btnWidth;
			pBtn->MoveWindow(curx, cMargin, btnWidth, btnHeight);
			curx -= cMargin;
		}
	}
	m_captionRect.right = curx;
	if (m_pContainedPane != NULL)
		m_pContainedPane->MovePane(0, m_captionHeight, cx, cy - m_captionHeight);

}

void TSPaneContainer::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_CAPTIONROTATION)
	{
		m_captionPos++;
		//CRect rc;
		//GetClientRect(&rc);
		//rc.bottom = m_captionHeight;
		InvalidateRect(&m_captionRect);
	}
}

void TSPaneContainer::SetFont(CFont* pFont, BOOL bRedraw/* = TRUE*/)
{
	CWnd::SetFont(pFont, bRedraw);
	ButtonCollection::iterator it = m_userButtons.begin();
	for (;it!=m_userButtons.end();it++)
		(*it)->SetFont(pFont, bRedraw);
	it = m_systemButtons.begin();
	for (;it!=m_systemButtons.end();it++)
		(*it)->SetFont(pFont, bRedraw);

	//CRect rc;
	//GetClientRect(&rc);
	//PositionControls(rc.Width(), rc.Height());
}


// CPlayListPane dialog
//#define CAPTION_TIMER_ID		100


//BOOL CTeenSpiritPane::Create(CWnd* pParent, UINT nID)
//{
//	if (CCaptionedPane::Create(pParent, nID))
//	{
//		SetCaptionRotationDelay(3000);
//		ApplySkin(PRGAPI()->GetSkinManager()->GetSkin());
//		return TRUE;
//	}
//	return FALSE;
//}

//void CTeenSpiritPane::ApplySkin(ISkin& _skin)
//{
//	TSSkin& skin = (TSSkin&) _skin;
//	SetFont(skin.GetFont(CSEC_Panes, FSTY_Normal), FALSE);
//	SetCaptionHeight(skin.GetIntParameter(CSEC_Panes, IPARAM_Height) + skin.GetRelativeFontSize());
//}
//
//BEGIN_MESSAGE_MAP(CTeenSpiritPane, CCaptionedPane)
//	ON_WM_TIMER()
//END_MESSAGE_MAP()
//
//void CTeenSpiritPane::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	if (nIDEvent == CAPTION_TIMER_ID)
//	{
//		BOOL bInvalidate = TRUE;
//		m_captionPos++;
//		if (GetTitle(m_captionPos) == NULL)
//		{
//			if (m_captionPos == 1)
//			{
//				//=== Pane has only one caption. No rotation is needed
//				m_captionPos = 0;
//				bInvalidate = FALSE;
//			}
//			else
//			{
//				//=== We ve reached the end of the captions
//				m_captionPos = 0;
//			}
//		}
//		else
//		{
//			//=== Caption exists
//		}
//		if (bInvalidate)
//		{
//			SetCaption(_T(""));
//		}
//	}
//	CCaptionedPane::OnTimer(nIDEvent);
//}
//
//BOOL CTeenSpiritPane::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	ASSERT(m_pPaneController != NULL);
//	if (m_pPaneController != NULL)
//	{
//		if (m_pPaneController->OnCommand(GetDlgCtrlID(), wParam) == TRUE)
//			return TRUE;
//	}
//	return CWnd::OnCommand(wParam, lParam);
//}
//
//void CTeenSpiritPane::SetCaptionRotationDelay(UINT ms)
//{
//	KillTimer(CAPTION_TIMER_ID);
//	if (ms > 0)
//		SetTimer(CAPTION_TIMER_ID, ms, NULL);
//}
//

void TSPaneContainer::OnNcDestroy()
{
	CWnd::OnNcDestroy();
	//m_pPaneController->ClosePane(*this);

}

void TSPaneContainer::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_pPaneController->MaxRestorePane(*this);
	__super::OnLButtonDblClk(nFlags, point);
}
