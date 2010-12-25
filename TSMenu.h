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
#ifndef _TSMenu_h_
#define _TSMenu_h_

#include "ITSMenu.h"
#include "GuiControls/BCMenu.h"

class TSMenu:public ITSMenu
{
public:
	TSMenu()				{}
	virtual ~TSMenu()		{}

	virtual BOOL Create();
	virtual void Destroy();

	virtual BOOL AppendMenu(MenuItemType mit, UINT cmdID, LPCTSTR text);
	virtual BOOL AppendMenu(MenuItemType mit, UINT cmdID, LPCTSTR text, HICON hIcon);
	virtual void CheckMenuItem(UINT cmdID, BOOL bChecked);
	virtual void EnableMenuItem(UINT cmdID, BOOL bEnable);
	virtual BOOL CheckMenuRadioItem(UINT idFirst, UINT idLast, UINT idCheck);

	virtual BOOL TrackPopupMenu(INT x, INT y, HWND hwndParent);

	virtual HMENU GetInternalHandler();

	virtual void SetDefaultItem(UINT cmdID);


	//virtual BOOL IsMenuItemChecked(UINT cmdID);
	//virtual BOOL IsMenuItemEnabled(UINT cmdID);

private:
	BCMenu m_menu;
};

#endif

