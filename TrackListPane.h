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
#include "ITranslation/ITranslation.h"
#include "TSSkin.h"
#include "IStateChangeListener.h"

#include "EnhTrackListWnd.h"

static PaneInfo TrackListPaneInfo = {_T("TrackListPane"), _T("Track List Pane"), _T(""), TRUE};


class TrackListPane :	public IPane,
	public TSTranslatable,			//=== It supports translations
	public IStateChangeListener		//=== It requires state change events
{
public:
	TrackListPane();
	virtual ~TrackListPane();

	virtual BOOL Init(HWND hwndParent);
	virtual void UnInit();

	virtual BOOL Show();
	virtual void Hide();
	virtual void MovePane(INT x, INT y, INT cx, INT cy);

	virtual LPCTSTR GetTitle(UINT captionNum);
	virtual BOOL GetButtonInfo(PaneButtonInfo& bInfo, UINT idx);
	virtual ITSMenu* GetMenu(UINT idx);

	virtual BOOL OnButton(UINT idx);

	virtual const PaneInfo& GetPaneInfo()						{return TrackListPaneInfo;}

	virtual BOOL LoadState(IPaneState& paneState);
	virtual BOOL SaveState(IPaneState& paneState);

	//=== public IStateChangeListener
	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()						{return TrackListPaneInfo.friendlyName;}

	//=== public ITranslatable
	virtual void ApplyTranslation(ITranslation& translation);



private:
	TCHAR m_stringBuffer[500];
	CEnhTrackListWnd m_enhTrackList;

	enum Buttons
	{
		CMD_Options = 0,
		CMD_Play,
		CMD_Enqueue,
		CMD_ShowInfoIcons,
		CMD_SyncWithPlayer,
		CMD_SyncWithSectionChanger,
		CMD_Last
	};

	//IPaneController* m_pPaneController;
	HWND m_hwndParent;
	CRect m_clientRC;
	ITSMenu* m_poptionsMenu;
	BOOL m_bSyncWithPlayer;
	BOOL m_bSyncWithSectionChanger;
};
