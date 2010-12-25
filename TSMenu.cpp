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
#include "TSMenu.h"
#include "PrgAPI.h"

BOOL TSMenu::Create()
{
	ASSERT(m_menu.m_hMenu == NULL);
	if (m_menu.m_hMenu != NULL)
		return TRUE;
	if (m_menu.CreatePopupMenu())
	{
		m_menu.SetFont(PRGAPI()->GetFont(FONT_Dialogs));
		m_menu.SetIconSize(16,16);
		return TRUE;
	}
	return FALSE;
}
void TSMenu::Destroy()
{
	//ASSERT(m_menu.m_hMenu != NULL);
	m_menu.DestroyMenu();
}

BOOL TSMenu::AppendMenu(MenuItemType mit, UINT cmdID, LPCTSTR text)
{
	ASSERT(m_menu.m_hMenu != NULL);
	return AppendMenu(mit, cmdID, text, 0);
}


BOOL TSMenu::AppendMenu(MenuItemType mit, UINT cmdID, LPCTSTR text, HICON hIcon)
{
	ASSERT(m_menu.m_hMenu != NULL);
	static const UINT mit2flag[] ={0, MF_STRING, MF_STRING | MF_POPUP, MF_SEPARATOR, 0};
	if (mit <= MIT_First || mit >= MIT_Last)
		return FALSE;
	return m_menu.AppendMenu(mit2flag[mit], cmdID, (LPTSTR)text, hIcon);
}
void TSMenu::CheckMenuItem(UINT cmdID, BOOL bChecked)
{
	ASSERT(m_menu.m_hMenu != NULL);
	m_menu.CheckMenuItem(cmdID, MF_BYCOMMAND | (bChecked ? MF_CHECKED : MF_UNCHECKED));
}
void TSMenu::EnableMenuItem(UINT cmdID, BOOL bEnabled)
{
	ASSERT(m_menu.m_hMenu != NULL);
	m_menu.EnableMenuItem(cmdID, MF_BYCOMMAND | (bEnabled ? MF_ENABLED : MF_GRAYED));
}

BOOL TSMenu::CheckMenuRadioItem(UINT idFirst, UINT idLast, UINT idCheck)
{
	ASSERT(m_menu.m_hMenu != NULL);
	return m_menu.CheckMenuRadioItem(idFirst, idLast, idCheck, MF_BYCOMMAND);
}


BOOL TSMenu::TrackPopupMenu(INT x, INT y, HWND hwndParent)
{
	ASSERT(m_menu.m_hMenu != NULL);
	return m_menu.TrackPopupMenu(TPM_LEFTALIGN, x, y, CWnd::FromHandle(hwndParent));
}


HMENU TSMenu::GetInternalHandler()
{
	return m_menu.m_hMenu;
}

void TSMenu::SetDefaultItem(UINT cmdID)
{
	BOOL bRet = m_menu.SetDefaultItem(cmdID, FALSE);
}


//BOOL TSMenu::IsMenuItemChecked(UINT cmdID)
//{
//	m_menu.Is
//}
//BOOL TSMenu::IsMenuItemEnabled(UINT cmdID)
//{
//
//}
