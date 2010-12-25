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
#include "WebPane.h"
#include "PrgAPI.h"
#include "resource.h"
#include "StateManager.h"
#include "ServicesRegistrationSrv.h"
#include "MenuManager.h"
#include "SQLManager.h"




enum Buttons
{
	BTN_Options,
	BTN_Last
};


BOOL m_bSyncWithPlayer;
BOOL m_bSyncWithSectionsChanger;
BOOL m_bSyncWithTracksChanger;

BOOL m_bUseAsWebViewer;


WebPane::WebPane():
m_hwndParent(0),
m_bSyncWithPlayer(TRUE),
m_bSyncWithSectionsChanger(TRUE),
m_bSyncWithTracksChanger(TRUE),
m_SyncWithArtistService(-1),
m_SyncWithAlbumService(-1),
m_SyncWithTrackService(-1)
{
	memset(&m_pMenu, 0, sizeof(m_pMenu));
}

WebPane::~WebPane()
{
	ResetAllMenus();
	for (int i = 0; i < M_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			delete m_pMenu[i];
	}
}

void WebPane::ResetAllMenus()
{
	for (int i = 0; i < M_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			m_pMenu[i]->Destroy();
	}
}



BOOL WebPane::Init(HWND hwndParent)
{
	if (m_web.m_hWnd != NULL)
		return TRUE;
	m_hwndParent = hwndParent;
	if (m_web.m_hWnd == NULL)
	{
		PrgAPI* pAPI = PRGAPI();
		CWnd* pParent = CWnd::FromHandle(hwndParent);
		//=== Create and configure the Tree Control
		if (m_web.Create(_T("#32770"), _T(""), WS_CHILD, CRect(0,0,0,0), pParent, 345) == FALSE)
			return FALSE;
		pAPI->GetStateManager()->RegisterStateChangeListener(*this);

		ServicesRegistrationSrv::IWebViewer* pWebViewer = pAPI->GetServicesRegistrationSrv()->GetWebViewer();
		if (pWebViewer == 0 && m_bUseAsWebViewer)
			pAPI->GetServicesRegistrationSrv()->SetWebViewer(this);
		return TRUE;
	}
	return FALSE;
}

void WebPane::UnInit()
{
	PrgAPI* pAPI = PRGAPI();
	pAPI->GetStateManager()->UnRegisterStateChangeListener(*this);
	ServicesRegistrationSrv::IWebViewer* pWebViewer = pAPI->GetServicesRegistrationSrv()->GetWebViewer();
	if (pWebViewer == this)
		pAPI->GetServicesRegistrationSrv()->SetWebViewer(NULL);
	m_web.DestroyWindow();
}
BOOL WebPane::Show()
{
	m_web.ShowWindow(SW_SHOW);
	return TRUE;
}
void WebPane::Hide()
{
	if (m_web.GetSafeHwnd())
		m_web.ShowWindow(SW_HIDE);
}


void WebPane::MovePane(INT x, INT y, INT cx, INT cy)
{
	m_web.MoveWindow(x, y, cx, cy, FALSE);
}


const LPCTSTR cWebPane_Header = _T("Header");
const LPCTSTR cWebPane_SyncWithServiceArtist = _T("SyncWithServiceArtist");
const LPCTSTR cWebPane_SyncWithServiceAlbum = _T("SyncWithServiceAlbum");
const LPCTSTR cWebPane_SyncWithServiceTrack = _T("SyncWithServiceTrack");


BOOL WebPane::LoadState(IPaneState& paneState)
{
	LPCTSTR sValue = paneState.GetLPCTSTRSetting(cWebPane_Header);
	if (_tcsicmp(sValue, WebPaneInfo.Name) != 0)
		return FALSE;
	m_SyncWithArtistService = paneState.GetIntSetting(cWebPane_SyncWithServiceArtist);
	m_SyncWithAlbumService = paneState.GetIntSetting(cWebPane_SyncWithServiceAlbum);
	m_SyncWithTrackService = paneState.GetIntSetting(cWebPane_SyncWithServiceTrack);
	return TRUE;
}



BOOL WebPane::SaveState(IPaneState& paneState)
{
	paneState.SetLPCTSTRSetting(cWebPane_Header, WebPaneInfo.Name);
	paneState.SetIntSetting(cWebPane_SyncWithServiceArtist, m_SyncWithArtistService);
	paneState.SetIntSetting(cWebPane_SyncWithServiceAlbum, m_SyncWithAlbumService);
	paneState.SetIntSetting(cWebPane_SyncWithServiceTrack, m_SyncWithTrackService);
	return TRUE;
}

BOOL WebPane::GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)
{
	switch (idx)
	{
	case BTN_Options:
		bInfo = PaneButtonInfo();
		bInfo.hIcon = PRGAPI()->GetIcon(ICO_Options16);
		bInfo.iconSize = 16;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
ITSMenu* WebPane::GetMenu(UINT idx)
{
	switch (idx)
	{
	case BTN_Options:
		{
			PrgAPI* pAPI = PRGAPI();
			//MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
			MenuManager* pMM = pAPI->GetMenuManager();
			//MenuCommandsEnum cmd = (MenuCommandsEnum) idx;
			ITSMenu& mnu = GetInternalMenu(M_Main);
			//pMM->ResetServicesMenu PlayerCommands(*pPlayer);
			//mnu.CheckMenuItem(MI_ShowNumbers, m_PlayList.GetConfigurationOption(CPlayListListCtrl::C_ShowNumbers) != 0);
			pMM->PrepareMenu(MenuManager::MNU_ArtistServices);
			pMM->PrepareMenu(MenuManager::MNU_AlbumServices);
			pMM->PrepareMenu(MenuManager::MNU_TrackServices);
			if (m_SyncWithArtistService > -1)
				mnu.CheckMenuItem(MENU_WebServicesHolder + m_SyncWithArtistService, TRUE);
			if (m_SyncWithAlbumService > -1)
				mnu.CheckMenuItem(MENU_WebServicesHolder + m_SyncWithAlbumService, TRUE);
			if (m_SyncWithTrackService > -1)
				mnu.CheckMenuItem(MENU_WebServicesHolder + m_SyncWithTrackService, TRUE);

			mnu.CheckMenuItem(MI_SyncWithPlayer, m_bSyncWithPlayer);
			mnu.CheckMenuItem(MI_SyncWithSectionChanger, m_bSyncWithSectionsChanger);
			mnu.CheckMenuItem(MI_SyncWithTracksChanger, m_bSyncWithTracksChanger);

			ServicesRegistrationSrv::IWebViewer* pViewer = pAPI->GetServicesRegistrationSrv()->GetWebViewer();
			mnu.CheckMenuItem(MI_UseAsWebViewer, pViewer == this);

			return &mnu;
		}
		break;
	default:
		return NULL;
	}
	return NULL;
}


BOOL WebPane::OnButton(UINT idx)
{
	if (idx >= MENU_WebServicesHolder && idx < MENU_WebServicesHolderEnd)
	{
		PrgAPI* pAPI = PRGAPI();
		ServicesRegistrationSrv* pSRS = pAPI->GetServicesRegistrationSrv();
		ServiceInfo si;
		INT serviceID = idx - MENU_WebServicesHolder;
		if (pSRS->GetServiceInfo(serviceID, si))
		{
			if (si.CanHandle(STYPE_ArtistInfo))
			{
				if (m_SyncWithArtistService == serviceID)
					m_SyncWithArtistService = -1;
				else
					m_SyncWithArtistService = serviceID;
			}
			else if (si.CanHandle(STYPE_AlbumInfo))
			{
				if (m_SyncWithAlbumService == serviceID)
					m_SyncWithAlbumService = -1;
				else
					m_SyncWithAlbumService = serviceID;
			}
			else if (si.CanHandle(STYPE_TrackInfo))
			{
				if (m_SyncWithTrackService == serviceID)
					m_SyncWithTrackService = -1;
				else
					m_SyncWithTrackService = serviceID;
			}
			else
				ASSERT(0);
		}
		return TRUE;
	}
	else
	{
		switch (idx)
		{
		case MI_SyncWithPlayer:
			m_bSyncWithPlayer = !m_bSyncWithPlayer;
			break;
		case MI_SyncWithSectionChanger:
			m_bSyncWithSectionsChanger= !m_bSyncWithSectionsChanger;
			break;
		case MI_SyncWithTracksChanger:
			m_bSyncWithTracksChanger = !m_bSyncWithTracksChanger;
			break;
		case MI_UseAsWebViewer:
			{
				ServicesRegistrationSrv* pSRS = PRGAPI()->GetServicesRegistrationSrv();
				ServicesRegistrationSrv::IWebViewer* pViewer = pSRS->GetWebViewer();
				if (pViewer != this)
					pSRS->SetWebViewer(this);
				else
					pSRS->SetWebViewer(NULL);
			}
			break;
		default:
			ASSERT(0);
			return FALSE;
		}
	}
	return TRUE;
}



LPCTSTR WebPane::GetTitle(UINT idx)
{
	PrgAPI* pAPI = PRGAPI();
	return pAPI->GetString(IDS_WEB);
}

BOOL WebPane::OpenURL(LPCTSTR url)
{
	if (m_web.GetSafeHwnd())
	{
		m_web.SetURL(url);
		return TRUE;
	}
	return FALSE;
}

ITSMenu& WebPane::GetInternalMenu(MenusEnum mnu)
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
				(INT)pMM->GetMenu(MenuManager::MNU_ArtistServices).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_ARTIST));

			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)pMM->GetMenu(MenuManager::MNU_AlbumServices).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_ALBUM));

			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)pMM->GetMenu(MenuManager::MNU_TrackServices).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_TITLE));
			
			curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
			
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_SyncWithSectionChanger, (LPTSTR)pAPI->GetString(IDS_SYNCWITHSECTIONS));
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_SyncWithTracksChanger, (LPTSTR)pAPI->GetString(IDS_SYNCWITHTRACKS));
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_SyncWithPlayer, (LPTSTR)pAPI->GetString(IDS_SYNCWITHPLAYER));
			
			curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
			
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_UseAsWebViewer, (LPTSTR)pAPI->GetString(IDS_WEBVIEWER));
		}
		break;
	default:
		ASSERT(0);
	}
	return curMenu;
}

BOOL WebPane::OnStateChanged(UINT stateMessages)
{
	BOOL bHandled = FALSE;
	switch (stateMessages)
	{
	case SM_CurrentSectionChanged:
		if (m_bSyncWithSectionsChanger)
		{
			PrgAPI* pAPI = PRGAPI();
			TSState& state = pAPI->GetStateManager()->GetState();
			ServicesRegistrationSrv* pSRS = pAPI->GetServicesRegistrationSrv();
			if (state.activeItemType == IT_Artist)
			{
				if (m_SyncWithArtistService != -1)
				{
					ServiceInfo si;
					if (pSRS->GetServiceInfo(m_SyncWithArtistService, si))
					{
						ArtistRecord rec;
						if (pAPI->GetSQLManager()->GetArtistRecord(state.activeTracksFilter.ArtistID.val, rec))
						{
							ServicesExecuteInfo inf;
							inf.artist = rec.name.c_str();
							pSRS->Execute(si, inf, this);
							bHandled = TRUE;
						}
					}
				}
			}
			else if (state.activeItemType == IT_Album)
			{
				if (m_SyncWithAlbumService != -1)
				{
					ServiceInfo si;
					if (pSRS->GetServiceInfo(m_SyncWithAlbumService, si))
					{
						FullAlbumRecordSP rec;
						if (pAPI->GetSQLManager()->GetFullAlbumRecordByID(rec, state.activeTracksFilter.AlbumID.val))
						{
							ServicesExecuteInfo inf;
							inf.artist = rec->artist.name.c_str();
							inf.album = rec->album.name.c_str();
							pSRS->Execute(si, inf, this);
							bHandled = TRUE;
						}
					}
				}
			}

		}
		break;
	case SM_CurrentTrackChanged:
		if (m_bSyncWithTracksChanger && m_SyncWithTrackService != -1)
		{
			PrgAPI* pAPI = PRGAPI();
			TSState& state = pAPI->GetStateManager()->GetState();
			ServicesRegistrationSrv* pSRS = pAPI->GetServicesRegistrationSrv();
			ServiceInfo si;
			if (pSRS->GetServiceInfo(m_SyncWithTrackService, si))
			{
				FullTrackRecordSP rec;
				if (pAPI->GetSQLManager()->GetFullTrackRecordByID(rec, state.trackListTrackID))
				{
					ServicesExecuteInfo inf;
					inf.artist = rec->artist.name.c_str();
					inf.album = rec->album.name.c_str();
					inf.title = rec->track.name.c_str();
					pSRS->Execute(si, inf, this);
					bHandled = TRUE;
				}
			}
		}
		break;
	case SM_MediaChanged:
		if (m_bSyncWithPlayer)
		{
			PrgAPI* pAPI = PRGAPI();
			TSState& state = pAPI->GetStateManager()->GetState();
			ServicesRegistrationSrv* pSRS = pAPI->GetServicesRegistrationSrv();
			ServiceInfo si;
			if (pSRS->GetServiceInfo(m_SyncWithTrackService, si))
			{
				FullTrackRecordSP rec;
				if (pAPI->GetSQLManager()->GetFullTrackRecordByID(rec, state.trackListTrackID))
				{
					ServicesExecuteInfo inf;
					inf.artist = rec->artist.name.c_str();
					inf.album = rec->album.name.c_str();
					inf.title = rec->track.name.c_str();
					pSRS->Execute(si, inf, this);
					bHandled = TRUE;
				}
			}
		}
		break;
	default:
		bHandled = FALSE;
	}
	return bHandled;
}
