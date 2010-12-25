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
#pragma once
#include "IPane.h"
#include "TSTranslatable.h"
#include "IStateChangeListener.h"
#include "TSBrowserEnh.h"
#include "ServicesRegistrationSrv.h"

static PaneInfo WebPaneInfo = {_T("WebPane"), _T("Web Pane"), _T("Internal"), TRUE};


class WebPane :	public IPane,
	public TSTranslatable,							//=== It supports translations
	public ServicesRegistrationSrv::IWebViewer,
	public IStateChangeListener						//=== It requires state change events
{
public:
	WebPane();   // standard constructor
	virtual ~WebPane();

	virtual BOOL Init(HWND hwndParent);
	virtual void UnInit();

	virtual BOOL Show();
	virtual void Hide();

	virtual void MovePane(INT x, INT y, INT cx, INT cy);

	virtual LPCTSTR GetTitle(UINT captionNum);
	virtual BOOL GetButtonInfo(PaneButtonInfo& bInfo, UINT idx);
	virtual ITSMenu* GetMenu(UINT idx);

	virtual BOOL OnButton(UINT idx);

	virtual const PaneInfo& GetPaneInfo()						{return WebPaneInfo;}

	virtual BOOL LoadState(IPaneState& paneState);
	virtual BOOL SaveState(IPaneState& paneState);

	//=== public ITranslatable
	virtual void ApplyTranslation(ITranslation& translation)	{ResetAllMenus();}

	//=== public IStateChangeListener
	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()						{return WebPaneInfo.Name;}

	virtual BOOL OpenURL(LPCTSTR url);

private:
	TSBrowserEnh m_web;
	enum MenuItemsEnum
	{
		MI_First = 0x2000,
		MI_SyncWithPlayer,
		MI_SyncWithSectionChanger,
		MI_SyncWithTracksChanger,
		MI_UseAsWebViewer,
		MI_Last
	};
	enum MenusEnum
	{
		M_Main,
		M_Last
	};
	ITSMenu* m_pMenu[M_Last];
	ITSMenu& GetInternalMenu(MenusEnum mnu);

	INT m_SyncWithArtistService;
	INT m_SyncWithAlbumService;
	INT m_SyncWithTrackService;

	BOOL m_bSyncWithPlayer;
	BOOL m_bSyncWithSectionsChanger;
	BOOL m_bSyncWithTracksChanger;

	BOOL m_bUseAsWebViewer;

private:
	//TCHAR m_stringBuffer[500];
	//CPlayListListCtrl m_PlayList;
	HWND m_hwndParent;

private:
	void ResetAllMenus();
	//ITSMenu& GetInternalMenu(MenusEnum mnu);
	//void ResetAllMenus();


};
