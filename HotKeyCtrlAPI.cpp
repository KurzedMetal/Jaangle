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
#include "HotKeyCtrlAPI.h"

void CHotKeyCtrlAPI::GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const
{
	WORD MFCmod;
	CHotKeyCtrl::GetHotKey(wVirtualKeyCode, MFCmod);
	//TRanslate to the API modifier
	wModifiers = (((MFCmod & HOTKEYF_ALT) == HOTKEYF_ALT) ? MOD_ALT : 0) |
		(((MFCmod & HOTKEYF_CONTROL) == HOTKEYF_CONTROL) ? MOD_CONTROL : 0) |
		(((MFCmod & HOTKEYF_EXT) == HOTKEYF_EXT) ? MOD_WIN : 0) |
		(((MFCmod & HOTKEYF_SHIFT) == HOTKEYF_SHIFT) ? MOD_SHIFT : 0);
}

DWORD CHotKeyCtrlAPI::GetHotKey() const
{
	WORD wVirtualKeyCode, wModifiers = 0;
	GetHotKey(wVirtualKeyCode, wModifiers);
	return MAKELONG(wVirtualKeyCode, wModifiers);
}


void CHotKeyCtrlAPI::SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
{
	WORD mod = 0;
	mod = (((wModifiers & MOD_ALT) == MOD_ALT) ? HOTKEYF_ALT : 0) |
		(((wModifiers & MOD_CONTROL) == MOD_CONTROL) ? HOTKEYF_CONTROL : 0) |
		(((wModifiers & MOD_WIN) == HOTKEYF_EXT) ? HOTKEYF_EXT : 0) |
		(((wModifiers & MOD_SHIFT) == MOD_SHIFT) ? HOTKEYF_SHIFT : 0);
	CHotKeyCtrl::SetHotKey(wVirtualKeyCode, mod);

}


BOOL CHotKeyCtrlAPI::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP && pMsg->hwnd == *this)
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_CHANGE), (LPARAM)GetSafeHwnd());
	return CHotKeyCtrl::PreTranslateMessage(pMsg);
}
