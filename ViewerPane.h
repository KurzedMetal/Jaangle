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

//#include "DirectorPat/BaseDirector.h"
//#include "TransmitterPattern/Transmitter.h"
#include "GuiControls/BCMenu.h"
//#include "CaptionedPane.h"

class CInfoCtrl;
class CVisualizationContainer;
class MediaPlayer;
class TSBrowserEnh;

class CViewerPane :	public IPane,
	//public CWnd,					//=== It is an MFC Window
	//public ITranslatable,			//=== It supports translations
	//public ISkinnable,				//=== It supports skins
	public IStateChangeListener		//=== It requires state change events
{
public:
	CViewerPane();   // standard constructor
	virtual ~CViewerPane();

	//=== IPane
	virtual BOOL Init(IPaneController* pPaneController, HWND hwndParent, UINT nID);
	virtual void UnInit();
	virtual BOOL Show();
	virtual void Hide();
	virtual void MovePane(INT x, INT y, INT cx, INT cy);
	virtual void SaveSettings();
	virtual LPCTSTR GetTitle(UINT captionNum);

	//=== public ITranslatable
	virtual void ApplyTranslation(ITranslation& translation);

	//=== public ISkinnable
	virtual void ApplySkin(ISkin& skin);

	//=== public IStateChangeListener
	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()				{return _T("CViewerPane");}

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
private:
	CInfoCtrl* m_pInfoCtrl;
	CVisualizationContainer* m_pVisCtrl;
	TSBrowserEnh* m_pBrowser;
	MODES m_mod;
	BOOL m_bInitialized;
	BOOL m_bSyncWithPlayer;
	BCMenu m_OptionsMenu;
	CInfoCtrl* GetInfoControl();
	TSBrowserEnh* GetBrowserCtrl();
	CVisualizationContainer* GetVisualizationCtrl();
	//BOOL m_bAutoViewerMode;
	//std::tstring m_caption;
private:
	void OnInit();
	//TCHAR m_stringBuffer[500];

private:
	TCHAR m_stringBuffer[500];


};
