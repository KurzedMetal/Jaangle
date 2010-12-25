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

#include "PlayListListCtrl.h"

static PaneInfo PlayListPaneInfo = {_T("PlayListPane"), _T("PlayList Pane"), _T("Internal"), TRUE};


class PlayListPane :	public IPane,
	public TSTranslatable,			//=== It supports translations
	public IStateChangeListener		//=== It requires state change events
{
public:
	PlayListPane();   // standard constructor
	virtual ~PlayListPane();

	virtual BOOL Init(HWND hwndParent);
	virtual void UnInit();

	virtual BOOL Show();
	virtual void Hide();

	virtual void MovePane(INT x, INT y, INT cx, INT cy);

	virtual LPCTSTR GetTitle(UINT captionNum);
	virtual BOOL GetButtonInfo(PaneButtonInfo& bInfo, UINT idx);
	virtual ITSMenu* GetMenu(UINT idx);

	virtual BOOL OnButton(UINT idx);

	virtual const PaneInfo& GetPaneInfo()						{return PlayListPaneInfo;}

	virtual BOOL LoadState(IPaneState& paneState);
	virtual BOOL SaveState(IPaneState& paneState);

	//=== public ITranslatable
	virtual void ApplyTranslation(ITranslation& translation)	{ResetAllMenus();}

	//=== public IStateChangeListener
	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()						{return _T("PlayListPane");}

private:
	enum MenuItemsEnum
	{
		MI_First = 0x2000,
		MI_ShowNumbers,
		MI_Last
	};
	enum MenusEnum
	{
		M_Main,
		M_Last
	};
	ITSMenu* m_pMenu[M_Last];

private:
	TCHAR m_stringBuffer[500];
	CPlayListListCtrl m_PlayList;
	HWND m_hwndParent;

private:
	ITSMenu& GetInternalMenu(MenusEnum mnu);
	void ResetAllMenus();


};
