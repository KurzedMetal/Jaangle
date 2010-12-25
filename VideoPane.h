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
#include "StateManager.h"
#include "VideoContainer.h"

class MediaPlayer;

static PaneInfo VideoPaneInfo = {_T("VideoPane"), _T("Video Pane"), _T(""), FALSE};

class VideoPane : public IPane,
	public ISkinnable,				//=== It supports skins
	public TSStateChangeListener		//=== It requires state change events
{
public:
	VideoPane();
	virtual ~VideoPane();

	virtual BOOL Init(HWND hwndParent);
	virtual void UnInit();

	virtual BOOL Show();
	virtual void Hide();
	virtual void MovePane(INT x, INT y, INT cx, INT cy);

	virtual LPCTSTR GetTitle(UINT captionNum);
	virtual BOOL GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)	{return FALSE;}
	virtual ITSMenu* GetMenu(UINT idx)							{return NULL;}

	virtual BOOL OnButton(UINT idx)								{return FALSE;}

	virtual const PaneInfo& GetPaneInfo()						{return VideoPaneInfo;}

	virtual BOOL LoadState(IPaneState& paneState);
	virtual BOOL SaveState(IPaneState& paneState);

	//=== public IStateChangeListener
	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()						{return VideoPaneInfo.friendlyName;}

	//=== public ISkinnable
	virtual void ApplySkin(ISkin& skin)							{}


	static BOOL IsCreatable();

private:
	static INT m_instances;
	CVideoContainer m_visCtrl;


};
