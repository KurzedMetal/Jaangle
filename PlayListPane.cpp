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
#include "PlayListPane.h"
#include "PlayListListCtrl.h"
#include "PrgAPI.h"
#include "TeenSpirit.h"
#include "MediaPlayer.h"
#include "ActionManager.h"
#include "MenuManager.h"
#include "ShortcutManager.h"
#include "StateManager.h"
#include "QuickSearchDlg.h"
//#include "StringSerializerHelper.h"


enum Buttons
{
	BTN_Options,
	BTN_QuickSearch,
	BTN_Last
};

PlayListPane::PlayListPane():
m_hwndParent(0)
{
	memset(&m_pMenu, 0, sizeof(m_pMenu));
}

PlayListPane::~PlayListPane()
{
	ResetAllMenus();
	for (int i = 0; i < M_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			delete m_pMenu[i];
	}
}

void PlayListPane::ResetAllMenus()
{
	for (int i = 0; i < M_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			m_pMenu[i]->Destroy();
	}
}



BOOL PlayListPane::Init(HWND hwndParent)
{
	if (m_PlayList.m_hWnd != NULL)
		return TRUE;
	m_hwndParent = hwndParent;
	if (m_PlayList.m_hWnd == NULL)
	{
		PrgAPI* pAPI = PRGAPI();
		CWnd* pParent = CWnd::FromHandle(hwndParent);
		//=== Create and configure the Tree Control
		if (m_PlayList.Create(WS_CHILD, CRect(0,0,0,0), pParent, 345) == FALSE)
			return FALSE;
		m_PlayList.EnableDragDrop(TRUE);
		m_PlayList.SetSingleSelection(FALSE);
		m_PlayList.GetScrollBar().SetIcons(pAPI->GetIcon(ICO_Up16), pAPI->GetIcon(ICO_Down16), 16, 16);
		pAPI->GetShortcutManager()->RegisterShortcutForFocus(VK_F4, m_PlayList.m_hWnd);
		m_PlayList.SetMediaPlayer(pAPI->GetMediaPlayer());
		pAPI->GetStateManager()->RegisterStateChangeListener(*this);
		return TRUE;
	}
	return FALSE;
}

void PlayListPane::UnInit()
{
	PRGAPI()->GetStateManager()->UnRegisterStateChangeListener(*this);
	m_PlayList.DestroyWindow();
}
BOOL PlayListPane::Show()
{
	m_PlayList.ShowWindow(SW_SHOW);
	return TRUE;
}
void PlayListPane::Hide()
{
	m_PlayList.ShowWindow(SW_HIDE);
}


void PlayListPane::MovePane(INT x, INT y, INT cx, INT cy)
{
	m_PlayList.MoveWindow(x, y, cx, cy, FALSE);
}

const LPCTSTR cPlayListPane_Header = _T("Header");
const LPCTSTR cPlayListPane_ShowNumbers = _T("ShowNumbers");


BOOL PlayListPane::LoadState(IPaneState& paneState)
{
	LPCTSTR sValue = paneState.GetLPCTSTRSetting(cPlayListPane_Header);
	if (_tcsicmp(sValue, PlayListPaneInfo.Name) != 0)
		return FALSE;
	INT value = paneState.GetIntSetting(cPlayListPane_ShowNumbers);
	if (value > -1)
		m_PlayList.SetConfigurationOption(CPlayListListCtrl::C_ShowNumbers, value);
	return TRUE;
}



BOOL PlayListPane::SaveState(IPaneState& paneState)
{
	paneState.SetLPCTSTRSetting(cPlayListPane_Header, PlayListPaneInfo.Name);
	paneState.SetIntSetting(cPlayListPane_ShowNumbers, m_PlayList.GetConfigurationOption(CPlayListListCtrl::C_ShowNumbers));
	return TRUE;
}

BOOL PlayListPane::GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)
{
	switch (idx)
	{
	case BTN_Options:
		bInfo = PaneButtonInfo();
		bInfo.hIcon = PRGAPI()->GetIcon(ICO_Options16);
		bInfo.iconSize = 16;
		break;
	case BTN_QuickSearch:
		bInfo = PaneButtonInfo();
		bInfo.hIcon = PRGAPI()->GetIcon(ICO_Find16);
		bInfo.iconSize = 16;
		break;
	default:
		return FALSE;
	}
	return TRUE;

}
ITSMenu* PlayListPane::GetMenu(UINT idx)
{
	switch (idx)
	{
	case BTN_Options:
		{
			PrgAPI* pAPI = PRGAPI();
			MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
			MenuManager* pMM = pAPI->GetMenuManager();
			MenuCommandsEnum cmd = (MenuCommandsEnum) idx;
			ITSMenu& mnu = GetInternalMenu(M_Main);
			pMM->PreparePlayerCommands(*pPlayer);
			mnu.CheckMenuItem(MI_ShowNumbers, m_PlayList.GetConfigurationOption(CPlayListListCtrl::C_ShowNumbers) != 0);
			return &mnu;
		}
		break;
	case BTN_QuickSearch:
		{
			PrgAPI* pAPI = PRGAPI();
			QuickSearchDlg* pQSDlg = pAPI->GetQuickSearchDlg();
			pQSDlg->SetSearchText(_T(""));
			pQSDlg->ShowRelative(AfxGetMainWnd(), QuickSearchDlg::RP_Centered);
			return NULL;
		}
		break;
	default:
		return NULL;
	}
	return NULL;
}


BOOL PlayListPane::OnButton(UINT idx)
{
	TRACEST(_T("PlayListPane::OnCommand"), idx);
	switch (idx)
	{
	case MI_ShowNumbers:
		m_PlayList.SetConfigurationOption(CPlayListListCtrl::C_ShowNumbers, 
			m_PlayList.GetConfigurationOption(CPlayListListCtrl::C_ShowNumbers) == 0 ? 1 : 0);
		break;
	default:
		{
			PrgAPI* pAPI = PRGAPI();
			MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
			MenuManager* pMM = pAPI->GetMenuManager();
			MenuCommandsEnum cmd = (MenuCommandsEnum) idx;
			if (!pMM->HandlePlayerCommands(*pPlayer, cmd, pPlayer->GetPlayListPos()) || (pMM->HandleGeneralCommands(cmd)))
				return FALSE;
		}
	}
	return TRUE;;
}



LPCTSTR PlayListPane::GetTitle(UINT idx)
{
	PrgAPI* pAPI = PRGAPI();
	switch (idx % 5)
	{
	case 0:
		return pAPI->GetString(IDS_PLAYLIST);
		break;//=== Show the default (at the end)
	case 1:
		{
			MediaPlayer* pl = pAPI->GetMediaPlayer();
			INT ACmode = 0;
			MediaPlayerNextMode mpm = pl->GetNextMode(&ACmode);
			if (mpm == MPNM_AutoContinue)
			{
				//INT mode = pAPI->GetOption(OPT_PL_AutoContinueMode);
				_sntprintf(m_stringBuffer, 500, _T("%s: %s [%s]"),
					pAPI->GetString(IDS_MODE),
					pAPI->GetStringForPlayListMode(mpm), 
					pAPI->GetStringForRandomMode(RandomTrackModeEnum(ACmode)));
			}
			else
				_sntprintf(m_stringBuffer, 500, _T("%s: %s"), 
					pAPI->GetString(IDS_MODE), 
					pAPI->GetStringForPlayListMode(MediaPlayerNextMode(mpm)));
		} 
		break;
	case 2:
		{
			MediaPlayer* pl = pAPI->GetMediaPlayer();
			INT totalTracks = pl->GetPlayListCount();
			_sntprintf(m_stringBuffer, 500, _T("%s: %d"), pAPI->GetString(IDS_TRACKS), totalTracks);
		} 
		break;
	case 3:
		{
			MediaPlayer* pl = pAPI->GetMediaPlayer();
			INT totalTracks = pl->GetPlayListCount();
			INT totalSeconds = 0;
			MediaPlayListItem mpli;
			BOOL bHasZeroLength = FALSE;
			for (INT i = 0; i < totalTracks; i++)
			{
				if (pl->GetPlayListItem(mpli, i))
				{
					if (mpli.length == 0)
						bHasZeroLength = TRUE;
					totalSeconds += mpli.length;
				}
			}
			TCHAR time[40];
			if (totalSeconds >= 3600)
				_sntprintf(time, 40, _T("%d:%02d:%02d"), INT (totalSeconds / 3600), INT((totalSeconds % 3600)/60), INT(totalSeconds) % 60);
			else
				_sntprintf(time, 40, _T("%d:%02d"), INT (totalSeconds / 60), INT(totalSeconds) % 60);

			_sntprintf(m_stringBuffer, 500, _T("%s: %s%s"), pAPI->GetString(IDS_DURATION),  time, bHasZeroLength ? _T(" +") : _T(""));
		} 
		break;
	case 4:
		_sntprintf(m_stringBuffer, 500, _T("%s: [F4]"), pAPI->GetString(IDS_FOCUS));
		break;
	default:
		ASSERT(0);
		return NULL;
	}
	return m_stringBuffer;
}

BOOL PlayListPane::OnStateChanged(UINT stateMessages)
{
	BOOL bHandled = TRUE;
	switch (stateMessages)
	{
	case SM_MediaChanged:
		m_PlayList.UpdateActiveItem();
		break;
	case SM_PlayListChanged:
		m_PlayList.UpdateList();
		break;
	default:
		bHandled = FALSE;
	}
	return bHandled;

}

ITSMenu& PlayListPane::GetInternalMenu(MenusEnum mnu)
{
	TRACEST(_T("PlayListPane::GetMenu."), mnu);
	ASSERT(mnu < M_Last);
	PrgAPI* pAPI = PRGAPI();
	if (m_pMenu[mnu] == NULL)
		m_pMenu[mnu] = pAPI->CreatePopupMenu();
	ITSMenu& curMenu = *m_pMenu[mnu];
	if (curMenu.GetInternalHandler() != NULL)
		return curMenu;
	curMenu.Create();
	switch (mnu)
	{
	case M_Main:
		{
			MenuManager* pMM = pAPI->GetMenuManager();
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)pMM->GetMenu(MenuManager::MNU_PlayListModes).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_MODE));
			
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)pMM->GetMenu(MenuManager::MNU_PlayListSerialization).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_PLAYLIST));

			curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_ShowNumbers, (LPTSTR)pAPI->GetString(IDS_SHOWNUMBERS));
		}
		break;
	default:
		ASSERT(0);
	}
	return curMenu;
}

