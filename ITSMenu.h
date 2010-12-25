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
#ifndef _ITSMenu_h_
#define _ITSMenu_h_

class ITSMenu
{
public:
	ITSMenu()				{}
	virtual ~ITSMenu()		{}

	virtual BOOL Create()		= 0;
	virtual void Destroy()		= 0;

	enum MenuItemType
	{
		MIT_First,
		MIT_String,
		MIT_Menu,
		MIT_Separator,
		MIT_Last
	};
	virtual BOOL AppendMenu(MenuItemType mit, UINT cmdID, LPCTSTR text)					= 0;
	virtual BOOL AppendMenu(MenuItemType mit, UINT cmdID, LPCTSTR text, HICON hIcon)	= 0;
	virtual void CheckMenuItem(UINT cmdID, BOOL bChecked)								= 0;
	virtual void EnableMenuItem(UINT cmdID, BOOL bEnable)								= 0;
	virtual BOOL CheckMenuRadioItem(UINT idFirst, UINT idLast, UINT idCheck)			= 0;

	virtual BOOL TrackPopupMenu(INT x, INT y, HWND hwndParent)							= 0;

	virtual HMENU GetInternalHandler()													= 0;

	virtual void SetDefaultItem(UINT cmdID)												= 0;

	//virtual BOOL IsMenuItemChecked(UINT cmdID)											= 0;
	//virtual BOOL IsMenuItemEnabled(UINT cmdID)											= 0;
};

#endif

