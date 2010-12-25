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
#include "InfoPane.h"
#include "MediaPlayer.h"
#include "PrgAPI.h"
#include "ShortcutManager.h"
#include "SQLManager.h"
#include "StringSerializerHelper.h"




InfoPane::InfoPane():
	m_bSyncWithPlayer(TRUE),
	m_bSyncWithSections(TRUE),
	m_bSyncWithTracks(TRUE),
	m_pOptionsMenu(NULL),
	m_pShowItemsMenu(NULL),
	m_pVideoContainer(NULL),
	m_hwndParent(0),
	m_bAutomaticallyShowVideo(TRUE)
{
}

InfoPane::~InfoPane()
{
	PrgAPI* pAPI = PRGAPI();
	//pAPI->SetOption(OPT_VIEWER_SyncWithPlayer, m_bSyncWithPlayer);
	if (m_infoCtrl.m_hWnd != NULL)
		m_infoCtrl.DestroyWindow();
	delete m_pOptionsMenu;
	delete m_pShowItemsMenu;
	delete m_pVideoContainer;
}

BOOL InfoPane::Init(HWND hwndParent)
{
	m_hwndParent = hwndParent;
	if (m_infoCtrl.GetSafeHwnd() != NULL)
		return TRUE;
	if (m_infoCtrl.Create(CWnd::FromHandle(hwndParent), 550))
	{
		//SetButton(IDC_OPTIONS, NULL, PRGAPI()->GetIcon(ICO_Options16), 16, 16, TRUE);
		//m_infoCtrl.Create(this, 902);
		//SetMainControl(&m_infoCtrl);
		HWND hwnd = m_infoCtrl.GetWindowForFocus();
		if (hwnd != NULL)
			PRGAPI()->GetShortcutManager()->RegisterShortcutForFocus(VK_F5, hwnd);	
		OnStateChanged(SM_MediaChanged);
		//m_infoCtrl.ShowWindow(SW_SHOW);
		return TRUE;
	}
	return FALSE;
}

void InfoPane::UnInit()
{
	if (m_infoCtrl.GetSafeHwnd())
	{
		HWND hwnd = m_infoCtrl.GetWindowForFocus();
		if (hwnd != NULL)
			PRGAPI()->GetShortcutManager()->UnRegisterShortcutForFocus(hwnd);		
		PRGAPI()->GetStateManager()->UnRegisterStateChangeListener(*this);
		m_infoCtrl.DestroyWindow();
	}
}


BOOL InfoPane::Show()
{
	if (m_infoCtrl.GetSafeHwnd())
	{
		if (m_infoCtrl.IsWindowVisible())
			return TRUE;
		PRGAPI()->GetStateManager()->RegisterStateChangeListener(*this);
		FullTrackRecordCollection& col = PRGAPI()->GetStateManager()->GetState().GetTrackList();
		if (col.size() > 0)
		{
			m_infoCtrl.SetActiveItem(col[0]);
			m_infoCtrl.SetStyle(IT_Album);
		}
		m_infoCtrl.ShowWindow(SW_SHOW);
		m_infoCtrl.Refresh();
		return TRUE;
	}
	return FALSE;
}

void InfoPane::Hide()
{
	if (m_infoCtrl.GetSafeHwnd())
		m_infoCtrl.ShowWindow(SW_HIDE);
}

void InfoPane::MovePane(INT x, INT y, INT cx, INT cy)
{
	m_infoCtrl.MoveWindow(x, y, cx, cy, FALSE);
	if (m_pVideoContainer != NULL)
		m_pVideoContainer->MoveWindow(x, y, cx, cy, FALSE);
}


LPCTSTR InfoPane::GetTitle(UINT captionNum)
{
	PrgAPI* pAPI = PRGAPI();
	switch (captionNum % 3)
	{
	case 0:
		return pAPI->GetString(IDS_SPECIALINFO);
	case 1:
		{
			const std::vector<InfoItemTypeEnum>& items = m_infoCtrl.GetShowingItems();
			if (items.size() == 0)
				_sntprintf(m_stringBuffer, 500, _T("%s: -"), pAPI->GetString(IDS_SHOW));
			else
			{
				_sntprintf(m_stringBuffer, 500, _T("%s: "), pAPI->GetString(IDS_SHOW));
				for (UINT i = 0; i < items.size(); i++)
				{
					if (i > 0)
						_tcscat(m_stringBuffer, _T(", "));
					_tcscat(m_stringBuffer, pAPI->GetStringForInfoItemType(items[i]));
				}

			}
			return m_stringBuffer;
		}
		break;
	case 2:
		_sntprintf(m_stringBuffer, 500, _T("%s: [F5]"), pAPI->GetString(IDS_FOCUS));
		return m_stringBuffer;
	}
	return NULL;

}
BOOL InfoPane::GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)
{
	switch (idx)
	{
	case BT_Options:
		bInfo.hIcon = PRGAPI()->GetIcon(ICO_Options16);
		bInfo.iconSize = 16;
		bInfo.text = NULL;
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL InfoPane::OnButton(UINT idx)
{
	switch (idx)
	{
	case MI_SyncWithPlayer:
		m_bSyncWithPlayer = !m_bSyncWithPlayer;
		break;
	case MI_SyncWithSectionChanger:
		m_bSyncWithSections = !m_bSyncWithSections;
		break;
	case MI_SyncWithTracksChanger:
		m_bSyncWithTracks = !m_bSyncWithTracks;
		break;
	case MI_AutomaticallyShowVideo:
		m_bAutomaticallyShowVideo = !m_bAutomaticallyShowVideo;
		OnStateChanged(SM_MediaChanged);
		break;
	default:
		if (idx > MI_ShowItemsStart && idx <= MI_ShowItemsEnd)
		{
			InfoItemTypeEnum iit = (InfoItemTypeEnum) (idx - MI_ShowItemsStart);
			m_infoCtrl.EnableInfoType(iit, !m_infoCtrl.IsInfoTypeEnabled(iit));
			m_infoCtrl.Refresh();
		}
		else
			return FALSE;
	}
	return TRUE;
}

const LPCTSTR cInfoPane_Header = _T("Header");
const LPCTSTR cInfoPane_SyncWithPlayer = _T("SyncWithPlayer");
const LPCTSTR cInfoPane_SyncWithSections = _T("SyncWithSections");
const LPCTSTR cInfoPane_SyncWithTracks = _T("SyncWithTracks");
const LPCTSTR cInfoPane_AutomaticallyShowVideo = _T("AutomaticallyShowVideo");
const LPCTSTR cInfoPane_EnableInfoItem = _T("EnableInfoItem");

void InfoPane::GetBoolValue(IPaneState& paneState, LPCTSTR value, BOOL& bValue)
{
	INT intValue = paneState.GetIntSetting(value);
	if (intValue != -1)
		bValue = (intValue != 0);
}

BOOL InfoPane::LoadState(IPaneState& paneState)
{
	LPCTSTR sValue = paneState.GetLPCTSTRSetting(cInfoPane_Header);
	if (_tcsicmp(sValue, InfoPaneInfo.Name) != 0)
		return FALSE;
	GetBoolValue(paneState, cInfoPane_SyncWithPlayer, m_bSyncWithPlayer);
	GetBoolValue(paneState, cInfoPane_SyncWithSections, m_bSyncWithSections);
	GetBoolValue(paneState, cInfoPane_SyncWithTracks, m_bSyncWithTracks);
	GetBoolValue(paneState, cInfoPane_AutomaticallyShowVideo, m_bAutomaticallyShowVideo);


	TCHAR bf[500];
	for (INT i = IIT_Unknown + 1; i < IIT_Last; i++)
	{
		_sntprintf(bf, 500, _T("%s_%d"), cInfoPane_EnableInfoItem, i);
		INT value = paneState.GetIntSetting(bf);
		if (value != -1)
			m_infoCtrl.EnableInfoType((InfoItemTypeEnum)i, value > 0);
	}
	return TRUE;
}
BOOL InfoPane::SaveState(IPaneState& paneState)
{
	paneState.SetLPCTSTRSetting(cInfoPane_Header, InfoPaneInfo.Name);
	paneState.SetIntSetting(cInfoPane_SyncWithPlayer, m_bSyncWithPlayer);
	paneState.SetIntSetting(cInfoPane_SyncWithSections, m_bSyncWithSections);
	paneState.SetIntSetting(cInfoPane_SyncWithTracks, m_bSyncWithTracks);
	paneState.SetIntSetting(cInfoPane_AutomaticallyShowVideo, m_bAutomaticallyShowVideo);

	TCHAR bf[500];
	for (INT i = IIT_Unknown + 1; i < IIT_Last; i++)
	{
		BOOL bEnable = FALSE;
		_sntprintf(bf, 500, _T("%s_%d"), cInfoPane_EnableInfoItem, i);
		paneState.SetIntSetting(bf, m_infoCtrl.IsInfoTypeEnabled((InfoItemTypeEnum)i));
	}
	return TRUE;
}


void InfoPane::ApplyTranslation(ITranslation& translation)
{
	if (m_pOptionsMenu != NULL)
	{
		m_pShowItemsMenu->Destroy();
		m_pOptionsMenu->Destroy();
	}
}

BOOL InfoPane::OnStateChanged(UINT stateMessages)
{
	BOOL bHandled = TRUE;
	switch (stateMessages)
	{
	case SM_CurrentSectionChanged:
		{
			if (m_bSyncWithSections)
			{
				PrgAPI* pAPI = PRGAPI();
				TSState& state = pAPI->GetStateManager()->GetState();
				m_infoCtrl.SetStyle(IT_Unknown);
				FullTrackRecordCollection& col = state.GetTrackList();
				if (col.size() > 0)
				{
					m_infoCtrl.SetActiveItem(col[0]);
					m_infoCtrl.SetStyle(state.activeItemType);
					m_infoCtrl.Refresh(100);
				}
			}
		}
		break;
	case SM_CurrentTrackChanged:
		if (m_bSyncWithTracks)
		{
			PrgAPI* pAPI = PRGAPI();
			TSState& state = pAPI->GetStateManager()->GetState();
			FullTrackRecordSP rec;
			if (pAPI->GetSQLManager()->GetFullTrackRecordByID(rec, state.trackListTrackID))
			{
				m_infoCtrl.SetStyle(IT_Track);
				m_infoCtrl.SetActiveItem(rec);
				m_infoCtrl.Refresh(100);
			}
		}
		break;
	case SM_MediaChanged:
		if (m_bSyncWithPlayer)
		{
			PrgAPI* pAPI = PRGAPI();
			MediaPlayer* pMP = pAPI->GetMediaPlayer();
			INT pos = pMP->GetPlayListPos();
			if (pos >= 0 && pos < (INT)pMP->GetPlayListCount())
			{
				MediaPlayListItem mpli;
				if (pMP->GetPlayListItem(mpli, pos))
				{
					FullTrackRecordSP rec;
					if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, mpli.url))
					{
						m_infoCtrl.SetStyle(IT_Track);
						m_infoCtrl.SetActiveItem(rec);
						m_infoCtrl.Refresh(100);
					}
				}

			}
		}
		if (m_bAutomaticallyShowVideo)
		{
			MediaPlayer* pMP = PRGAPI()->GetMediaPlayer();
			if (pMP->IsVideo())
			{
				//=== It is a video. Check if there is a viewer attached to the player
				if (pMP->GetVideoContainerHWND() == NULL)
				{
					//=== Media Player DO NOT have a video viewer attached
					//=== DECISION: We will attach our Viewer
					ASSERT(m_pVideoContainer == NULL);
					m_pVideoContainer = new CVideoContainer;
					if (m_pVideoContainer->Create( NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), CWnd::FromHandle(m_hwndParent), 920, NULL))
					{
						m_pVideoContainer->Init(pMP);
						CRect rc;
						m_infoCtrl.GetWindowRect(&rc);
						ScreenToClient(m_hwndParent, &rc.TopLeft());
						ScreenToClient(m_hwndParent, &rc.BottomRight());
						m_pVideoContainer->MoveWindow(&rc);
						m_infoCtrl.ShowWindow(SW_HIDE);
					}
					else
					{
						//=== FAILED for some reason
						ASSERT(0);
						delete m_pVideoContainer;
						m_pVideoContainer = NULL;
					}
				}
				else
				{
					//=== Media Player has already a video viewer attached
					if (m_pVideoContainer == NULL)
					{
						//=== It is not ours...
						//=== DECISION: Do nothing ... leave the InfoControl...
					}
					else
					{
						//=== It should be OUR player (playing from a previous track)
						//=== Decision: Do nothing... let it continue play

					}
				}


			}
			else
			{
				//=== It is not a video
				//=== Clear the videoContainer
				if (m_pVideoContainer != NULL)
				{
					m_pVideoContainer->DestroyWindow();
					delete m_pVideoContainer;
					m_pVideoContainer = NULL;
					m_infoCtrl.ShowWindow(SW_SHOW);
				}
			}
		}
		else
		{
			//=== It is not a video
			//=== Clear the videoContainer
			if (m_pVideoContainer != NULL)
			{
				m_pVideoContainer->DestroyWindow();
				delete m_pVideoContainer;
				m_pVideoContainer = NULL;
				m_infoCtrl.ShowWindow(SW_SHOW);
			}

		}
		break;
	case SM_InfoChanged:
	case SM_PictureChanged:
		m_infoCtrl.Refresh();
		break;
	default:
		bHandled = FALSE;
	}
	return bHandled;
}




ITSMenu* InfoPane::GetMenu(UINT idx)
{
	if (idx == BT_Options)
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_pOptionsMenu == 0)
		{
			m_pShowItemsMenu = pAPI->CreatePopupMenu();
			m_pOptionsMenu = pAPI->CreatePopupMenu();
		}
		if (m_pOptionsMenu->GetInternalHandler() == 0)
		{
			m_pShowItemsMenu->Create();
			for (INT i = IIT_Unknown + 1; i < IIT_Last; i++)
			{
				switch (i)
				{
				case IIT_GenrePicture:
				case IIT_YearPicture:
				case IIT_CollectionPicture:
				case IIT_MonthPicture:
					break;
				default:
					m_pShowItemsMenu->AppendMenu(ITSMenu::MIT_String, MI_ShowItemsStart + i, (LPTSTR)pAPI->GetStringForInfoItemType((InfoItemTypeEnum)i));
				}
			}

			m_pOptionsMenu->Create();
			m_pOptionsMenu->AppendMenu(ITSMenu::MIT_Menu, (UINT) m_pShowItemsMenu->GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_SHOW));	
			m_pOptionsMenu->AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
			m_pOptionsMenu->AppendMenu(ITSMenu::MIT_String, MI_SyncWithPlayer, (LPTSTR)pAPI->GetString(IDS_SYNCWITHPLAYER));
			m_pOptionsMenu->AppendMenu(ITSMenu::MIT_String, MI_SyncWithSectionChanger, (LPTSTR)pAPI->GetString(IDS_SYNCWITHSECTIONS));
			m_pOptionsMenu->AppendMenu(ITSMenu::MIT_String, MI_SyncWithTracksChanger, (LPTSTR)pAPI->GetString(IDS_SYNCWITHTRACKS));
			m_pOptionsMenu->AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
			m_pOptionsMenu->AppendMenu(ITSMenu::MIT_String, MI_AutomaticallyShowVideo, (LPTSTR)pAPI->GetString(IDS_VIDEO));
		};

		for (INT i = IIT_Unknown + 1; i < IIT_Last; i++)
		{
			m_pShowItemsMenu->CheckMenuItem(MI_ShowItemsStart + i, 
				MF_BYCOMMAND | 
				(m_infoCtrl.IsInfoTypeEnabled(InfoItemTypeEnum(i)) ? MF_CHECKED : MF_UNCHECKED)
				);
		}
		m_pOptionsMenu->CheckMenuItem(MI_SyncWithPlayer, MF_BYCOMMAND | (m_bSyncWithPlayer ? MF_CHECKED : MF_UNCHECKED));
		m_pOptionsMenu->CheckMenuItem(MI_SyncWithSectionChanger, MF_BYCOMMAND | (m_bSyncWithSections ? MF_CHECKED : MF_UNCHECKED));
		m_pOptionsMenu->CheckMenuItem(MI_SyncWithTracksChanger, MF_BYCOMMAND | (m_bSyncWithTracks ? MF_CHECKED : MF_UNCHECKED));
		m_pOptionsMenu->CheckMenuItem(MI_AutomaticallyShowVideo, MF_BYCOMMAND | (m_bAutomaticallyShowVideo ? MF_CHECKED : MF_UNCHECKED));

		return m_pOptionsMenu;
	}
	return NULL;
}




//BOOL InfoPane::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	TRACEST(_T("CViewerPane::OnCommand"), wParam);
//	PrgAPI* pAPI = PRGAPI();
//	switch (wParam)
//	{
//	case IDC_OPTIONS:
//		{
//			m_OptionsMenu.CheckMenuItem(IDS_SYNCWITHPLAYER, m_bSyncWithPlayer ? MF_CHECKED : MF_UNCHECKED);
//			RECT rc;
//			GetDlgItem(wParam)->GetWindowRect(&rc);
//			m_OptionsMenu.TrackPopupMenu(TPM_LEFTALIGN, rc.left + ((rc.right - rc.left) / 2), rc.bottom, this);
//			return 0;
//		}
//		break;
//	case IDS_SYNCWITHPLAYER:
//		m_bSyncWithPlayer = !m_bSyncWithPlayer;
//		break;
//	default:
//		return CTeenSpiritPane::OnCommand(wParam, lParam);
//	}
//	return 0;
//}

//BEGIN_MESSAGE_MAP(InfoPane, CTeenSpiritPane)
//	ON_WM_SIZE()
//END_MESSAGE_MAP()
//
//void InfoPane::OnSize(UINT nType, int cx, int cy)
//{
//	CTeenSpiritPane::OnSize(nType, cx, cy);
//	if (m_infoCtrl.m_hWnd != 0)
//		m_infoCtrl.MoveWindow(0,0,cx,cy,TRUE);
//}
