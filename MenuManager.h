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
#ifndef _MenuManager_h_
#define _MenuManager_h_

#include "TSTranslatable.h"
#include "TeenSpiritEnums.h"
//#include "GuiControls/BCMenu.h"
#include "ITSMenu.h"

class MediaPlayer;
	

#define MENU_WebServicesMAX 100

enum MenuCommandsEnum
{
	MENU_First = 0x1600,
	MENU_Exit,
	MENU_ShowMainWindow,
	MENU_HideMiniPlayer,
	MENU_ShowMiniPlayer,
	//MENU_ToggleMiniPlayer,
	MENU_LoadList,
	MENU_InsertList,
	MENU_SaveList,
	//MENU_ShowTrack,
	//MENU_RemoveSelected,
	//MENU_RemoveOthers,
	//MENU_RemoveAll,
	//MENU_RandomizeList,
	MENU_PlayControl,
	MENU_PauseControl,
	MENU_StopControl,
	MENU_NextControl,
	MENU_PrevControl,
	MENU_PlayFullAlbum,
	MENU_ShowAboutDlg,
	MENU_ShowHistoryDlg,
	MENU_ShowGamesDlg,
	MENU_ShowAdvancedSearch,
	MENU_ShowOptionsDlg,
	MENU_ShowCollectionsDlg,
	MENU_UpdateLocalCollections,
	MENU_ShowSkinsDialog,
	MENU_ShowLanguagesDialog,

	MENU_AutoContinueModeHolder,
	MENU_AutoContinueModeHolderEnd = MENU_AutoContinueModeHolder + 0xF,
	//MENU_InsertRandomTracksHolder,
	//MENU_InsertRandomTracksHolderEnd = MENU_InsertRandomTracksHolder + 0xF,
	//MENU_SearchTracksHolder,
	//MENU_SearchTracksHolderEnd = MENU_SearchTracksHolder + 0xF,
	MENU_PlayModesHolder,
	MENU_PlayModesHolderEnd = MENU_PlayModesHolder + 0xF,
	MENU_WebServicesHolder,
	MENU_WebServicesHolderEnd = MENU_WebServicesHolder + 0x4F,
	MENU_RatingHolder,
	MENU_RatingHolderEnd = MENU_RatingHolder + 0xF,
	MENU_SectionModesHolder,
	MENU_SectionModesHolderEnd = MENU_SectionModesHolder + 0xF,
	MENU_SectionFiltersHolder,
	MENU_SectionFiltersHolderEnd = MENU_SectionFiltersHolder + 0xF,
	MENU_SectionStylesHolder,
	MENU_SectionStylesHolderEnd = MENU_SectionStylesHolder + 0xF,
	MENU_SectionGroupingHolder,
	MENU_SectionGroupingHolderEnd = MENU_SectionGroupingHolder + 0xF,
	MENU_RenamerOptions,
	MENU_RenamerOptionsEnd = MENU_RenamerOptions + 0x1F,
	MENU_ReadTagsFromFilename,
	MENU_ReadTagsFromFilenameEnd = MENU_ReadTagsFromFilename + 0x1F,

	MENU_Last
};




class MenuManager: public TSTranslatable
{
public:
	enum MENUS
	{
		MNU_PlayListModes,
		MNU_AutoContinueMode,
		MNU_PlayListSerialization,
		MNU_Tray,
		//MNU_MiniPlayer,
		MNU_TrackServices,
		MNU_AlbumServices,
		MNU_ArtistServices,
		MNU_Rating,
		MNU_PlayerControl,
		MNU_Last
	};

	MenuManager();
	~MenuManager();

	ITSMenu* CreateMenu();
	void ResetAllMenus();


	ITSMenu& GetMenu(MENUS mnu);

	void PreparePlayerCommands(MediaPlayer& player);
	void PrepareMenu(MENUS menu);

	BOOL HandlePlayerCommands(MediaPlayer& player, MenuCommandsEnum cmd, INT playlistPos);
	BOOL HandleGeneralCommands(MenuCommandsEnum cmd);


	virtual void ApplyTranslation(ITranslation& skin); //ICtrlStrings
private:
	ITSMenu* m_pMenu[MNU_Last];

private:
	//void PreparePlayListModesMenu(ITSMenu& mnu, MediaPlayer* pPlayer);
	//void PrepareAutoContinueModeMenu(ITSMenu& mnu, MediaPlayer* pPlayer);
	//void PrepareTrayMenu(ITSMenu& mnu);
	//void PrepareMiniPlayerMenu(ITSMenu& mnu, MediaPlayer* pPlayer);
	//void AppendPlayerMenu(ITSMenu& mnu);
	//void AppendPlayerControlsMenu(ITSMenu& mnu);
	void AppendServicesMenu(ITSMenu& mnu, ItemTypeEnum it);

};

#endif
