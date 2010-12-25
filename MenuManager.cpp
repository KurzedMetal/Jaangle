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
#include "MenuManager.h"
#include "ActionManager.h"
#include "PrgAPI.h"
#include "resource.h"
#include "ServicesRegistrationSrv.h"
#include "SQLManager.h"
#include "CollectionManager.h"
#include "TSMenu.h"
#include "MiniPlayerDlg.h"
#include "SearchDlg.h"



MenuManager::MenuManager()
{
	memset(&m_pMenu, 0, sizeof(m_pMenu));
}
MenuManager::~MenuManager()
{
	ResetAllMenus();
	for (int i = 0; i < MNU_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			delete m_pMenu[i];
	}

}

void MenuManager::ApplyTranslation(ITranslation& translation)
{
	ResetAllMenus();
}

ITSMenu* MenuManager::CreateMenu()
{
	return new TSMenu;
}



void MenuManager::ResetAllMenus()
{
	for (int i = 0; i < MNU_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			m_pMenu[i]->Destroy();
	}
}


ITSMenu& MenuManager::GetMenu(MENUS mnu)
{
	TRACEST(_T("MenuManager::GetMenu."), mnu);
	ASSERT(mnu < MNU_Last);
	PrgAPI* pAPI = PRGAPI();
	if (m_pMenu[mnu] == NULL)
		m_pMenu[mnu] = pAPI->CreatePopupMenu();
	ITSMenu& curMenu = *m_pMenu[mnu];
	if (curMenu.GetInternalHandler() != NULL)
		return curMenu;
	curMenu.Create();
	switch (mnu)
	{
	case MNU_AutoContinueMode:
		for (int i = RTM_All; i < RTM_Last; i++)
		{
			curMenu.AppendMenu(ITSMenu::MIT_String, MENU_AutoContinueModeHolder + i, (LPTSTR)pAPI->GetStringForRandomMode((RandomTrackModeEnum)i));
			if (i == RTM_All)
				curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		}
		break;
	case MNU_PlayListSerialization:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_InsertList, (LPTSTR)pAPI->GetString(IDS_INSERTLIST));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_LoadList, (LPTSTR)pAPI->GetString(IDS_LOADLIST));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SaveList, (LPTSTR)pAPI->GetString(IDS_SAVELIST));
		break;
	//case MNU_InsertRandomTracks:
	//	for (int i = RTM_All; i < RTM_Last; i++)
	//	{
	//		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_InsertRandomTracksHolder + i, (LPTSTR)pAPI->GetStringForRandomMode((RandomTrackModeEnum)i));
	//		if (i == RTM_All)
	//			curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
	//	}
	//	break;
	//case MNU_SearchTracks:
	//	for (int i = 0; i < TTT_Last; i++)
	//		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SearchTracksHolder + i, (LPTSTR)pAPI->GetStringForTrackTextType((TrackTextType)i));
	//	break;
	case MNU_Tray:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_ShowMainWindow, (LPTSTR)pAPI->GetString(IDS_SHOWMAINWINDOW));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_ShowMiniPlayer, pAPI->GetString(IDS_SHOWMINIPLAYER));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_PlayerControl).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_MEDIAPLAYER));
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_PlayListModes).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_AUTOCONTINUEMODE));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_Exit, (LPTSTR)pAPI->GetString(IDS_EXIT), pAPI->GetIcon(ICO_Close16));
		curMenu.SetDefaultItem(MENU_ShowMainWindow);
		break;
	case MNU_PlayerControl:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_PlayControl, (LPTSTR)pAPI->GetString(IDS_PLAY), pAPI->GetIcon(ICO_Play24));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_PauseControl, (LPTSTR)pAPI->GetString(IDS_PAUSE), pAPI->GetIcon(ICO_Pause24));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_StopControl, (LPTSTR)pAPI->GetString(IDS_STOP), pAPI->GetIcon(ICO_Stop24));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_NextControl, (LPTSTR)pAPI->GetString(IDS_NEXT), pAPI->GetIcon(ICO_Next24));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_PrevControl, (LPTSTR)pAPI->GetString(IDS_PREVIOUS), pAPI->GetIcon(ICO_Prev24));
		curMenu.SetDefaultItem(MENU_PlayControl);
		break;
	//case MNU_MiniPlayer:
	//	curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_PlayListModes).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_PLAYLIST));
	//	curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_Rating).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_RATING), pAPI->GetIcon(ICO_StarGold16));
	//	curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
	//	curMenu.AppendMenu(ITSMenu::MIT_String, MENU_ShowMainWindow, (LPTSTR)pAPI->GetString(IDS_SHOWMAINWINDOW));
	//	curMenu.AppendMenu(ITSMenu::MIT_String, MENU_HideMiniPlayer, (LPTSTR)pAPI->GetString(IDS_HIDE));
	//	curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
	//	curMenu.AppendMenu(ITSMenu::MIT_String, MENU_Exit, (LPTSTR)pAPI->GetString(IDS_EXIT));
	//	break;
	case MNU_TrackServices:
		AppendServicesMenu(curMenu, IT_Track);
		break;
	case MNU_AlbumServices:
		AppendServicesMenu(curMenu, IT_Album);
		break;
	case MNU_ArtistServices:
		AppendServicesMenu(curMenu, IT_Artist);
		break;
	case MNU_Rating:
		for (int i = 5; i >= 0 ; i--)
		{
			if (i==0)
				curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
			curMenu.AppendMenu(ITSMenu::MIT_String, MENU_RatingHolder + i, 
				(LPTSTR) pAPI->GetStringForStars(i), pAPI->GetIconForStars(i));
		}
		break;
	case MNU_PlayListModes:
		for (int i = 0; i < MPNM_Last; i++)
			curMenu.AppendMenu(ITSMenu::MIT_String, MENU_PlayModesHolder + i, (LPTSTR)pAPI->GetStringForPlayListMode(MediaPlayerNextMode(i)));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_AutoContinueMode).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_AUTOCONTINUEMODE));
		break;
	//case MNU_StartMenu:
	//	curMenu.SetIconSize(32,32);
		//curMenu.AppendMenu(MF_STRING, MENU_ShowAdvancedSearch, (LPTSTR) pAPI->GetString(IDS_ADVSEARCH), pAPI->GetIcon(ICO_Search32));
		//curMenu.AppendMenu(MF_STRING, MENU_ShowHistoryDlg, (LPTSTR) pAPI->GetString(IDS_HISTORY), pAPI->GetIcon(ICO_History32));
		//curMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		//curMenu.AppendMenu(MF_STRING, MENU_ShowOptionsDlg, (LPTSTR) pAPI->GetString(IDS_OPTIONS), pAPI->GetIcon(ICO_Options32));
		//curMenu.AppendMenu(MF_STRING, MENU_ShowSkinsDialog, (LPTSTR) pAPI->GetString(IDS_SKIN));
		//curMenu.AppendMenu(MF_STRING, MENU_ShowLanguagesDialog, (LPTSTR) pAPI->GetString(IDS_LANGUAGE));
		//curMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		//curMenu.AppendMenu(MF_STRING, MENU_ShowCollectionsDlg, (LPTSTR) pAPI->GetString(IDS_COLLECTIONS), pAPI->GetIcon(ICO_Collections32));
		//curMenu.AppendMenu(MF_STRING, MENU_UpdateLocalCollections, (LPTSTR) pAPI->GetString(IDS_UPDATELOCALCOLLECTIONS));
		//curMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		//curMenu.AppendMenu(MF_STRING, MENU_ShowGamesDlg, (LPTSTR) pAPI->GetString(IDS_GAMES), pAPI->GetIcon(ICO_Games32));
		//curMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		//curMenu.AppendMenu(MF_STRING, MENU_ShowAboutDlg, (LPTSTR) pAPI->GetString(IDS_ABOUT), pAPI->GetIcon(ICO_About32));
		break;
	default:
		ASSERT(0);
	}
	return curMenu;
}


//void MenuManager::AppendPlayerMenu(ITSMenu& mnu)
//{
//	PrgAPI* pAPI = PRGAPI();
//	mnu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_AutoContinueMode).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_AUTOCONTINUEMODE));
//	mnu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
//	mnu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_PlayListSerialization).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_PLAYLIST));
//	mnu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
//	mnu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_Rating).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_RATING));
//	mnu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
//	mnu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetMenu(MenuManager::MNU_PlayListOptions).GetInternalHandler(), (LPTSTR)PRGAPI()->GetString(IDS_OPTIONS));
//}



void MenuManager::AppendServicesMenu(ITSMenu& mnu, ItemTypeEnum it)
{
	PrgAPI* pAPI = PRGAPI();
	ServicesRegistrationSrv* pSRS = pAPI->GetServicesRegistrationSrv();
	TCHAR appPath[MAX_PATH];
	GetModuleFileName(0, appPath, MAX_PATH);
	LPTSTR pos = _tcsrchr(appPath, '\\');
	pos[0] = 0;
	INT added = 0;
	for (INT i = 0; i < (INT)pSRS->GetServicesCount(); i++)
	{
		ServiceTypeEnum st = STYPE_Unknown;
		switch (it)
		{
		case IT_Artist:
			st = STYPE_ArtistInfo;
			break;
		case IT_Album:
			st = STYPE_AlbumInfo;
			break;
		case IT_Track:
			st = STYPE_TrackInfo;
			break;
		default:
			ASSERT(0);
			break;
		}
		ServiceInfo si;
		if (pSRS->GetServiceInfo(i, si) && si.CanHandle(st))
		{
			if ((added-1) % 5 == 0 && added > 1)
				mnu.AppendMenu(ITSMenu::MIT_Separator, 0, 0);
			added++;
			mnu.AppendMenu(ITSMenu::MIT_String, MENU_WebServicesHolder + i, (LPTSTR)si.Name, si.hIcon);
		}
	}
}

void MenuManager::PrepareMenu(MENUS menu)
{
	ITSMenu* pMenu = m_pMenu[menu];
	if (pMenu == NULL)
		return;
	switch (menu)
	{
	case MNU_Tray:
		{
			BOOL bWndVisible = AfxGetMainWnd()->IsWindowVisible();
			pMenu->EnableMenuItem(MENU_ShowMainWindow, bWndVisible == FALSE);

			BOOL bIsPlayerVisible = FALSE;
			CMiniPlayerDlg* pMPDlg = PRGAPI()->GetMiniPlayerDlg(FALSE);
			if (pMPDlg != NULL)
				bIsPlayerVisible = pMPDlg->IsWindowVisible();
			pMenu->EnableMenuItem(MENU_ShowMiniPlayer, bIsPlayerVisible == FALSE);
		}
		break;
	case MNU_ArtistServices:
	case MNU_AlbumServices:
	case MNU_TrackServices:
		for (INT i = MENU_WebServicesHolder; i < MENU_WebServicesHolderEnd; i++)
			pMenu->CheckMenuItem(i, FALSE);
		break;
	}
}




void MenuManager::PreparePlayerCommands(MediaPlayer& player)
{
	ITSMenu& mnu = GetMenu(MNU_PlayListModes);
	INT mode = 0;
	MediaPlayerNextMode mpm = player.GetNextMode(&mode);
	for (int i = MPNM_Normal; i < MPNM_Last; i++)
		mnu.CheckMenuItem(MENU_PlayModesHolder + i, mpm == i ? MF_CHECKED : MF_UNCHECKED );
	for (int i = RTM_All; i < RTM_Last; i++)
		mnu.CheckMenuItem(MENU_AutoContinueModeHolder + i, mode == i ? MF_CHECKED : MF_UNCHECKED );
}

BOOL MenuManager::HandlePlayerCommands(MediaPlayer& player, MenuCommandsEnum cmd, INT playlistPos)
{
	TRACEST(_T("MenuManager::HandlePlayerCommands"));
	PrgAPI* pAPI = PRGAPI();
	ActionManager* pAM = pAPI->GetActionManager();
	switch (cmd)
	{
		//=== Play Control =========================================
	case MENU_PlayControl:
		player.Start();
		break;
	case MENU_PauseControl:
		if (player.GetPlayState() == PS_Paused)
			player.Start();
		else
			player.Pause();
		break;
	case MENU_StopControl:
		player.Stop();
		break;
	case MENU_NextControl:
		player.Next();
		break;
	case MENU_PrevControl:
		player.Previous();
		break;
		//=== Playlist Serialization =========================================
	case MENU_LoadList:
		pAM->InsertPlayList(&player, -1);
		break;
	case MENU_SaveList:
		pAM->SavePlayList(&player);
		break;
	case MENU_InsertList:
		pAM->InsertPlayList(&player, playlistPos);
		break;
	default:
		//=== PlayModes =========================================
		if (cmd>= MENU_PlayModesHolder && cmd < MENU_PlayModesHolder + MPNM_Last)
			player.SetNextMode(MediaPlayerNextMode(cmd - MENU_PlayModesHolder), -1);
		else if (cmd >= MENU_AutoContinueModeHolder && cmd < MENU_AutoContinueModeHolder + RTM_Last)
		{
			player.SetNextMode(player.GetNextMode(NULL), cmd - MENU_AutoContinueModeHolder);
		}
		//=== Rating =========================================
		else if (cmd>= MENU_RatingHolder && cmd <= MENU_RatingHolder + 5)
		{
			int idx = playlistPos;
			if (idx >= 0)
			{
				SQLManager* pSM = PRGAPI()->GetSQLManager();
				MediaPlayListItem mpli;
				if (player.GetPlayListItem(mpli, idx))
				{
					FullTrackRecordSP rec;
					if (pSM->GetFullTrackRecordByLocation(rec, mpli.url))
					{
						FullTrackRecordCollection col;
						col.push_back(rec);
						ActionManager* pAM = PRGAPI()->GetActionManager();
						pAM->SetTrackRating(col, Stars2Rating((FLOAT)(cmd - MENU_RatingHolder)));
					}
				}
			}
		}

		else
			return FALSE;
	}
	return TRUE;
}

BOOL MenuManager::HandleGeneralCommands(MenuCommandsEnum cmd)
{
	TRACEST(_T("MenuManager::HandleGeneralCommands"), cmd);
	PrgAPI* pAPI = PRGAPI();
	ActionManager* pAM = pAPI->GetActionManager();
	switch (cmd)
	{
	case MENU_Exit:
		AfxGetMainWnd()->PostMessage(WM_QUIT);
		break;
	case MENU_ShowMainWindow:
		pAM->ShowMainWindow();
		break;
	case MENU_ShowMiniPlayer:
		pAPI->GetMiniPlayerDlg(TRUE)->ShowWindow(TRUE);
		break;
	case MENU_HideMiniPlayer:
		{
			CMiniPlayerDlg* pMPDlg = pAPI->GetMiniPlayerDlg(FALSE);
			if (pMPDlg)
				pMPDlg->ShowWindow(SW_HIDE);
		}
		break;
	//case MENU_ToggleMiniPlayer:
	//	pAPI->GetActionManager()->ShowMiniPlayer(!pAPI->GetActionManager()->IsMiniPlayerVisible());
	//	break;
	case MENU_ShowAboutDlg:
		pAM->ShowAboutDlg(AfxGetMainWnd());
		break;
	case MENU_ShowHistoryDlg:
		pAM->ShowHistoryDlg(AfxGetMainWnd());
		break;
	case MENU_ShowGamesDlg:
		pAM->ShowGamesDlg(AfxGetMainWnd());
		break;
	case MENU_ShowAdvancedSearch:
		pAPI->GetAdvancedSearchDlg()->ShowWindow(SW_SHOW);
		break;
	case MENU_ShowOptionsDlg:
		pAM->ShowOptionsDlg(AfxGetMainWnd());
		break;
	case MENU_ShowCollectionsDlg:
		pAM->ShowCollectionsDlg(AfxGetMainWnd());
		break;
	case MENU_UpdateLocalCollections:
		pAPI->GetCollectionManager()->RefreshLocalCollections(TRUE, FALSE, TRUE);
		break;
	case MENU_ShowSkinsDialog:
		pAM->ShowSkinsDlg(AfxGetMainWnd());
		break;
	case MENU_ShowLanguagesDialog:
		pAM->ShowLanguagesDlg(AfxGetMainWnd());
		break;
	default:
		return FALSE;
	}
	return TRUE;

}
//
//ITSMenu& MenuManager::GetPlayListItemMenu(MediaPlayer* pPlayer, INT SelectedItemsCount, INT nSelectedItem)
//{
//	ITSMenu& mnu = GetMenu(MNU_PlayListItem);
//	pMM->PreparePlayListMenu(playListMenu, pAPI->GetMediaPlayer());
//
//	ITSMenu& insertTracksMenu = pMM->GetMenu(MenuManager::MNU_InsertRandomTracks);
//	for (int i = RTM_All + 1; i < RTM_Last; i++)
//		insertTracksMenu.EnableMenuItem(MENU_InsertRandomTracksHolder + i, !(m_MenuInitiator == -1 && i != RTM_Newest));
//	ITSMenu& searchTracksMenu = pMM->GetMenu(MenuManager::MNU_SearchTracks);
//	for (int i = RTM_All + 1; i < RTM_Last; i++)
//		searchTracksMenu.EnableMenuItem(MENU_SearchTracksHolder + i, !(m_MenuInitiator == -1 && i != RTM_Newest));
//
//
//	playListMenu.EnableMenuItem(MENU_ShowTrack,  m_MenuInitiator != -1);
//	playListMenu.EnableMenuItem(MENU_RemoveOthers, GetSelectedItemCount() > 0);
//	playListMenu.EnableMenuItem(MENU_RemoveSelected, GetSelectedItemCount() > 0);
//
//
//}









