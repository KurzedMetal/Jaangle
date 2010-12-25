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
#ifndef _IPane_h_
#define _IPane_h_

#include "ITSMenu.h"
class IPaneController;

struct PaneButtonInfo
{
	PaneButtonInfo():text(0), hIcon(NULL),iconSize(0)				{}
	LPCTSTR text; 
	HICON hIcon;
	UINT iconSize;
};

struct PaneInfo
{
	LPCTSTR Name;
	LPCTSTR friendlyName;
	LPCTSTR author;
	BOOL bAllowMultipleInstances;
};

class IPaneState
{
public:
	IPaneState()				{}
	virtual ~IPaneState()		{}

	virtual INT GetIntSetting(LPCTSTR settingName)							= 0;
	virtual void SetIntSetting(LPCTSTR settingName, INT value)				= 0;
	virtual LPCTSTR GetLPCTSTRSetting(LPCTSTR settingName)					= 0;
	virtual void SetLPCTSTRSetting(LPCTSTR settingName, LPCTSTR value)		= 0;

};

class IPane
{
public:
	IPane()					{}
	virtual ~IPane()		{}

	virtual BOOL Init(HWND hwndParent)										= 0;
	virtual void UnInit()													= 0;

	virtual BOOL Show()														= 0;
	virtual void Hide()														= 0;

	virtual void MovePane(INT x, INT y, INT cx, INT cy)						= 0;

	virtual LPCTSTR GetTitle(UINT rollingIdx)								= 0;
	virtual BOOL GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)				= 0;
	virtual ITSMenu* GetMenu(UINT idx)										= 0;
	virtual BOOL OnButton(UINT idx)											= 0;

	virtual const PaneInfo& GetPaneInfo()									= 0;

	virtual BOOL LoadState(IPaneState& paneState)							= 0;
	virtual BOOL SaveState(IPaneState& paneState)							= 0;

};

class IPaneCreator
{
public:
	IPaneCreator()					{}
	virtual ~IPaneCreator()			{}

	virtual IPane* CreateInstance()											= 0;
	virtual const PaneInfo& GetPaneInfo()									= 0;
	virtual BOOL IsCreatable()												= 0;
};


#endif
