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
#include "ToolbarWnd.h"
#include "PrgApi.h"
#include "hdcUtils.h"
#include "ActionManager.h"
#include "CollectionManager.h"

#define ID_RELATIVE 530


// CMainToolbarDlg dialog

CToolbarWnd::CToolbarWnd(CWnd* pParent /*=NULL*/)
	:m_ToolbarHeight(0),
	m_bPassedOnEraseBk(FALSE)

{
}

CToolbarWnd::~CToolbarWnd()
{
	ClearMenu();
	if (m_hWnd) 
		DestroyWindow();
}

void CToolbarWnd::ClearMenu()
{
	m_collectionsMenu.DestroyMenu();
	m_aboutMenu.DestroyMenu();
	m_optionsMenu.DestroyMenu();

}

BEGIN_MESSAGE_MAP(CToolbarWnd, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CToolbarWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CWnd::Create(_T("STATIC"), NULL, dwStyle | SS_NOTIFY, rect, pParentWnd, nID, NULL) == FALSE)
		return FALSE;

	m_Search.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_Search);
	m_History.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_History);
	m_Options.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_Options);
	m_Collections.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_Collections);
	m_Games.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_Games);
	m_About.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_About);

	//m_decreaseFontSize.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_DecreaseFontSize);
	//m_resetFontSize.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_ResetFontSize);
	//m_increaseFontSize.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_IncreaseFontSize);
	m_setFontSize.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_SetFontSize);

	m_min.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_Min);
	m_restore.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_Restore);
	m_close.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, CMD_Close);


	ConfigureButton(m_Search, ICO_Find16);
	ConfigureButton(m_History, ICO_History16);
	ConfigureButton(m_Options, ICO_Options16);
	ConfigureButton(m_Collections, ICO_Collections16);
	m_Collections.SetComponents(FlexButton::COMP_Image | FlexButton::COMP_Text | FlexButton::COMP_DropDown);
	ConfigureButton(m_Games, ICO_Games16);
	ConfigureButton(m_About, ICO_Main16);
	m_About.SetComponents(FlexButton::COMP_Image | FlexButton::COMP_Text | FlexButton::COMP_DropDown);

	//ConfigureRightButtons(m_decreaseFontSize, ICO_DecreaseFont16);
	//ConfigureRightButtons(m_resetFontSize, ICO_ResetFont16);
	//ConfigureRightButtons(m_increaseFontSize, ICO_IncreaseFont16);
	ConfigureRightButtons(m_setFontSize, ICO_ResetFont16);

	ConfigureRightButtons(m_min, ICO_Minimize16);
	ConfigureRightButtons(m_restore, ICO_Restore16);
	ConfigureRightButtons(m_close, ICO_Close16);

	PrgAPI* pAPI = PRGAPI();
	ApplyTranslation(*pAPI->GetTranslationManager());
	ApplySkin(pAPI->GetSkinManager()->GetSkin());
	return TRUE;
}



void CToolbarWnd::ApplyTranslation(ITranslation& translation)
{
	PrgAPI* pApi = PRGAPI();
	m_Search.SetWindowText(pApi->GetString(IDS_ADVSEARCH));
	m_History.SetWindowText(pApi->GetString(IDS_HISTORY));
	m_Options.SetWindowText(pApi->GetString(IDS_OPTIONS));
	m_Collections.SetWindowText(pApi->GetString(IDS_COLLECTIONS));
	m_Games.SetWindowText(pApi->GetString(IDS_GAMES));
	m_About.SetWindowText(CTeenSpiritApp::sAppTitle);
	CRect rc;
	GetClientRect(&rc);

	PositionControls(rc.Width(), rc.Height());
	m_collectionsMenu.DestroyMenu();
	ClearMenu();
}

void CToolbarWnd::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	CFont* pFont = skin.GetFont(CSEC_Toolbar, FSTY_Normal);

	m_Search.SetFont(pFont);
	m_History.SetFont(pFont);
	m_Options.SetFont(pFont);
	m_Collections.SetFont(pFont);
	m_Games.SetFont(pFont);
	m_About.SetFont(pFont);
	CRect rc;
	GetClientRect(&rc);
	PositionControls(rc.Width(), rc.Height());


	SetColor(COL_Bk, skin.GetColor(CSEC_Toolbar, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_Toolbar, CVAL_Text));
	SetColor(COL_ButtonBk, skin.GetColor(CSEC_ToolbarButtons, CVAL_Bk));
	SetColor(COL_ButtonText, skin.GetColor(CSEC_ToolbarButtons, CVAL_Text));
	SetColor(COL_ButtonOverBk, skin.GetColor(CSEC_ToolbarButtons, CVAL_OverBk));
	SetColor(COL_ButtonOverText, skin.GetColor(CSEC_ToolbarButtons, CVAL_OverText));
	SetColor(COL_ButtonDownBk, skin.GetColor(CSEC_ToolbarButtons, CVAL_DownBk));
	SetColor(COL_ButtonDownText, skin.GetColor(CSEC_ToolbarButtons, CVAL_DownText));

	m_fontSizeMenu.DestroyMenu();

	Invalidate();
}




const int cMargins = 1;
const int cNormalButtonWidth = 90; 


void CToolbarWnd::ConfigureButton(FlexButton& btn, APP_ICONS ico)
{
	btn.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	btn.SetTextDrawEffect(FlexButton::ST_Over, FlexButton::TDE_Over);
	//btn.SetTextDrawEffect(FlexButton::ST_Down, FlexButton::TDE_Down);
	btn.SetBackgroundDrawEffect(FlexButton::ST_Over, FlexButton::BDE_Over);
	btn.SetBackgroundDrawEffect(FlexButton::ST_Down, FlexButton::BDE_Down);

	btn.SetTextStyle(FlexButton::TS_SingleLine);
	PrgAPI* pAPI = PRGAPI();
	if (ico != ICO_Last)
		btn.SetIcon(pAPI->GetIcon(ico), 16, 16);
}

void CToolbarWnd::ConfigureRightButtons(FlexButton& btn, APP_ICONS ico)
{
	btn.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	btn.SetComponents(FlexButton::COMP_Image);
	PrgAPI* pAPI = PRGAPI();
	if (ico != ICO_Last)
		btn.SetIcon(pAPI->GetIcon(ico), 16, 16);
}


void CToolbarWnd::PositionControls(INT cx, INT cy)
{
	if (m_Search.m_hWnd == 0)
		return;
	UINT ySize = cy - 2 * cMargins;
	UINT curX = cMargins;

	curX = PositionButtonLeft(m_Search, curX , ySize);
	curX = PositionButtonLeft(m_History, curX, ySize);
	curX = PositionButtonLeft(m_Options, curX, ySize);
	curX = PositionButtonLeft(m_Collections, curX, ySize);
	curX = PositionButtonLeft(m_Games, curX, ySize);
	curX = PositionButtonLeft(m_About, curX , ySize);

	const int cIconButtonWidth = 20;
	curX = cx - cIconButtonWidth - cMargins;
	if (GetParent()->IsZoomed())
	{
		m_close.MoveWindow(curX, cMargins, cIconButtonWidth, ySize);
		curX -= (cIconButtonWidth + cMargins);
		m_restore.MoveWindow(curX, cMargins, cIconButtonWidth, ySize);
		curX -= (cIconButtonWidth + cMargins);
		m_min.MoveWindow(curX, cMargins, cIconButtonWidth, ySize);
		curX -= (cIconButtonWidth + cMargins);
		curX -= cMargins;
		m_close.ShowWindow(SW_SHOW);
		m_restore.ShowWindow(SW_SHOW);
		m_min.ShowWindow(SW_SHOW);
	}
	else
	{
		m_close.ShowWindow(SW_HIDE);
		m_restore.ShowWindow(SW_HIDE);
		m_min.ShowWindow(SW_HIDE);
	}
	m_setFontSize.MoveWindow(curX, cMargins, cIconButtonWidth, ySize);
	//m_increaseFontSize.MoveWindow(curX, cMargins, cIconButtonWidth, ySize);
	//curX -= (cIconButtonWidth + cMargins);
	//m_resetFontSize.MoveWindow(curX, cMargins, cIconButtonWidth, ySize);
	//curX -= (cIconButtonWidth + cMargins);
	//m_decreaseFontSize.MoveWindow(curX, cMargins, cIconButtonWidth, ySize);


	Invalidate(TRUE);//=== Otherwise it will have artifacts.
}


//Returns the right edge
INT CToolbarWnd::PositionButtonLeft(FlexButton& btn, INT curLeft, INT Height)
{
	INT minWidth = btn.CalcMinWidth();
	if (minWidth < cNormalButtonWidth)	minWidth = cNormalButtonWidth;
	btn.MoveWindow(curLeft, cMargins, minWidth, Height);
	return curLeft + minWidth + cMargins;
}

void CToolbarWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_hWnd != NULL)
	{
		m_ToolbarHeight = cy;

		if (m_Search.m_hWnd == NULL)
		{

		}
	}
	if (m_bPassedOnEraseBk)
		PositionControls(cx, cy);

}



BOOL CToolbarWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("CMainToolbarDlg::OnCommand wParam: %d\r\n"), wParam);
	PrgAPI* pAPI = PRGAPI();
	switch (wParam)
	{
	case CMD_Collections:
		if (m_Collections.IsMultiClicked())
		{
			if (m_collectionsMenu.m_hMenu == 0)
			{
				PrgAPI* pAPI = PRGAPI();
				m_collectionsMenu.CreatePopupMenu();
				m_collectionsMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				m_collectionsMenu.AppendMenu(MF_STRING, ID_UPDATE, (LPTSTR)pAPI->GetString(IDS_UPDATELOCALCOLLECTIONS), pAPI->GetIcon(ICO_Refresh16));
			}
			m_Collections.DisplayPopupMenu(&m_collectionsMenu);
			return TRUE;
		}
		break;
	case CMD_About:
		if (m_About.IsMultiClicked())
		{
			if (m_aboutMenu.m_hMenu == 0)
			{
				PrgAPI* pAPI = PRGAPI();
				m_aboutMenu.CreatePopupMenu();

				m_aboutMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				TCHAR bf[300];
				_sntprintf(bf, 300, _T("%s..."), pAPI->GetString(IDS_PERSONALPAGE));
				m_aboutMenu.AppendMenu(MF_STRING, ACMD_GotoPersonalPage, bf, pAPI->GetIcon(ICO_Internet16));
				m_aboutMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				_sntprintf(bf, 300, _T("%s..."), pAPI->GetString(IDS_WEBSITE));
				m_aboutMenu.AppendMenu(MF_STRING, ACMD_GotoWebSite, bf, pAPI->GetIcon(ICO_Internet16));
				_sntprintf(bf, 300, _T("%s..."), pAPI->GetString(IDS_FORUM));
				m_aboutMenu.AppendMenu(MF_STRING, ACMD_GotoForum, bf, pAPI->GetIcon(ICO_Internet16));
				_sntprintf(bf, 300, _T("%s..."), pAPI->GetString(IDS_DONATION));
				m_aboutMenu.AppendMenu(MF_STRING, ACMD_GotoDonation, bf, pAPI->GetIcon(ICO_Internet16));
				m_aboutMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_aboutMenu.AppendMenu(MF_STRING, ACMD_CheckForUpdates, (LPTSTR)pAPI->GetString(IDS_CHECKFORUPDATES));
			}
			m_About.DisplayPopupMenu(&m_aboutMenu);
			return TRUE;
		}
		break;
	case CMD_Options:
		if (m_Options.IsMultiClicked())
		{
			if (m_optionsMenu.m_hMenu == 0)
			{
				PrgAPI* pAPI = PRGAPI();
				m_optionsMenu.CreatePopupMenu();
				m_optionsMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				//m_optionsMenu.AppendMenu(MF_STRING, ID_UPDATE, (LPTSTR)pAPI->GetString(IDS_UPDATELOCALCOLLECTIONS));
			}
			m_Options.DisplayPopupMenu(&m_optionsMenu);
			return TRUE;
		}
		break;
	//case CToolbarWnd::CMD_IncreaseFontSize:
	//	pAPI->GetSkinManager()->IncreaseRelativeFontSize();
	//	break;
	//case CToolbarWnd::CMD_DecreaseFontSize:
	//	pAPI->GetSkinManager()->DecreaseRelativeFontSize();
	//	break;
	case CMD_SetFontSize:
		{
			TSSkin& skin = pAPI->GetSkinManager()->GetSkin();
			INT minRelative = skin.GetIntParameter(CSEC_Global, IPARAM_MinRelativeFont);
			INT maxRelative = skin.GetIntParameter(CSEC_Global, IPARAM_MaxRelativeFont);
			if (m_fontSizeMenu.m_hMenu == NULL)
			{
				m_fontSizeMenu.CreatePopupMenu();
				m_fontSizeMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				for (INT i = maxRelative; i >= -minRelative; i--)
				{
					TCHAR bf[20];
					_sntprintf(bf, 20, _T("%s%d"), i > 0 ? _T("+") : _T(""), i);
					m_fontSizeMenu.AppendMenu(MF_STRING, ID_RELATIVE + i + minRelative, bf);
					if (i == 0 || i == 1)
						m_fontSizeMenu.AppendMenu(MF_SEPARATOR, 0, 0);

				}
			}
			for (INT i = maxRelative; i >= -minRelative; i--)
				m_fontSizeMenu.CheckMenuItem(ID_RELATIVE + i, MF_UNCHECKED | MF_BYCOMMAND);
			m_fontSizeMenu.CheckMenuItem(ID_RELATIVE + skin.GetRelativeFontSize() + minRelative, MF_CHECKED | MF_BYCOMMAND);
			m_setFontSize.DisplayPopupMenu(&m_fontSizeMenu);
		}
		break;
	case ACMD_GotoPersonalPage:
		pAPI->GoToSiteURL(PrgAPI::SA_Navigate, _T("tsonlineservices"));
		return TRUE;
	case ACMD_GotoWebSite:
		pAPI->GoToSiteURL(PrgAPI::SA_Navigate, _T("home"));
		return TRUE;
	case ACMD_GotoForum:
		pAPI->GoToSiteURL(PrgAPI::SA_Navigate, _T("forum"));
		return TRUE;
	case ACMD_GotoDonation:
		pAPI->GoToSiteURL(PrgAPI::SA_Navigate, _T("donate"));
		return TRUE;
	case ACMD_CheckForUpdates:
		pAPI->GetActionManager()->CheckForUpdates(this);
		return TRUE;
	case ID_UPDATE:
		return pAPI->GetCollectionManager()->RefreshLocalCollections(TRUE, FALSE, TRUE);
	default:
		if (wParam >= ID_RELATIVE && wParam < ID_RELATIVE + 100)
		{
			SkinManager* pSM = pAPI->GetSkinManager();
			TSSkin& skin = pSM->GetSkin();
			INT minRelative = skin.GetIntParameter(CSEC_Global, IPARAM_MinRelativeFont);
			INT newRelativeSkinSize = wParam - ID_RELATIVE - minRelative;
			if (newRelativeSkinSize != skin.GetRelativeFontSize())
			{
				skin.SetRelativeFontSize(newRelativeSkinSize);
				pSM->UpdateSkinnables(skin);
			}
			return TRUE;
		}
	}

	GetParent()->SendMessage(WM_COMMAND, wParam, 0);
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CToolbarWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, m_colors[COL_Bk]);//RGB(230,230,250));
	if (m_bPassedOnEraseBk == FALSE)
	{
		m_bPassedOnEraseBk = TRUE;
		PositionControls(rc.Width(), rc.Height());
	}
	return TRUE;//CDialog::OnEraseBkgnd(pDC);
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CToolbarWnd::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		FlexButton::COLOR buttonIndexColor = FlexButton::COL_Last;
		switch (idx)
		{
		case COL_ButtonBk:
			buttonIndexColor = FlexButton::COL_Bk;
			break;
		case COL_ButtonDownBk:
			buttonIndexColor = FlexButton::COL_Down;
			break;
		case COL_ButtonOverBk:
			buttonIndexColor = FlexButton::COL_Over;
			break;
		case COL_ButtonText:
			buttonIndexColor = FlexButton::COL_Text;
			break;
		case COL_ButtonDownText:
			buttonIndexColor = FlexButton::COL_DownText;
			break;
		case COL_ButtonOverText:
			buttonIndexColor = FlexButton::COL_OverText;
			break;
		}
		m_colors[idx] = value;
		if (buttonIndexColor != FlexButton::COL_Last)
		{
			m_Search.SetColor(buttonIndexColor, value);
			m_History.SetColor(buttonIndexColor, value);
			m_Options.SetColor(buttonIndexColor, value);
			m_Collections.SetColor(buttonIndexColor, value);
			m_Games.SetColor(buttonIndexColor, value);
			m_About.SetColor(buttonIndexColor, value);

			//m_decreaseFontSize.SetColor(buttonIndexColor, value);
			//m_resetFontSize.SetColor(buttonIndexColor, value);
			//m_increaseFontSize.SetColor(buttonIndexColor, value);
			m_setFontSize.SetColor(buttonIndexColor, value);

			m_min.SetColor(buttonIndexColor, value);
			m_restore.SetColor(buttonIndexColor, value);
			m_close.SetColor(buttonIndexColor, value);
		}
		if (m_hWnd)		
			Invalidate();
	}
}
COLORREF CToolbarWnd::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
//const TCHAR* const cColorNames[] = {
//	_T("Bk"),
//	_T("Text"),
//	_T("ButtonBk"),
//	_T("ButtonText"),
//	_T("ButtonOver"),
//	_T("ButtonOverText"),
//	_T("ButtonDown"),
//	_T("ButtonDownText"),
//	NULL
//};
//
//LPCTSTR CToolbarWnd::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------

void CToolbarWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	AfxGetMainWnd()->SendMessage(WM_SYSCOMMAND, AfxGetMainWnd()->IsZoomed() ? SC_RESTORE : SC_MAXIMIZE, 0);

	__super::OnLButtonDblClk(nFlags, point);
}

void CToolbarWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}
