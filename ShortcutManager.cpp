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
#include "StdAfx.h"
#include "ShortcutManager.h"
#include "PrgAPI.h"
#include "MediaPlayer.h"
#include "StateManager.h"


ShortcutManager::ShortcutManager()
{

}
ShortcutManager::~ShortcutManager()
{

}

void ShortcutManager::Init(HWND mainWindow)
{
	m_mainWindow = mainWindow;	
}

void ShortcutManager::RegisterShortcutForFocus(INT vk, HWND hwndToSetFocus)
{
	ASSERT(m_mainWindow != NULL);
	ASSERT(hwndToSetFocus != NULL);
	if (hwndToSetFocus == NULL)
		return;
	m_focusMap[vk] = hwndToSetFocus;
}

void ShortcutManager::UnRegisterShortcutForFocus(HWND hwnd)
{
	ASSERT(m_mainWindow != NULL);
	ASSERT(hwnd != NULL);
	if (hwnd == NULL)
		return;
	std::map<INT, HWND>::iterator it = m_focusMap.begin();
	for (; it != m_focusMap.end(); it++)
	{
		if (it->second == hwnd)
		{
			m_focusMap.erase(it);
			break;
		}
	}
}

ShortcutManager::ActionRequiredEnum ShortcutManager::HandleKeyMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//=== Focus window controls
	ASSERT(m_mainWindow != NULL);
	if (message == WM_KEYUP)
	{
		std::map<INT, HWND>::const_iterator it = m_focusMap.find(wParam);
		if (it != m_focusMap.end())
		{
			HWND focusHwnd = ::GetFocus();
			if (focusHwnd != NULL)
			{
				if (m_mainWindow == focusHwnd || m_mainWindow == GetAncestor(focusHwnd, GA_ROOT))
				{
					SetFocus((*it).second);
					return AR_ReturnFalse;
				}
			}
		}
	}

	//=== ALT+F4
	if (message == WM_SYSKEYDOWN && wParam == VK_F4)
	{
		HWND focusHwnd = ::GetFocus();
		if (focusHwnd != NULL)
		{
			if (m_mainWindow == focusHwnd || m_mainWindow == GetAncestor(focusHwnd, GA_ROOT))
			{
				SendMessage(m_mainWindow, WM_CLOSE,0,0);
				return AR_ReturnFalse;
			}
		}
	}

	//=== Leave all messages pass if we are in an edit
	TCHAR bf[10];
	if (GetClassName(hwnd, bf, 10))
	{
		if (_tcsnicmp(bf, _T("EDIT"), 4) == 0)
			return AR_DoNothing;
	}

	//=== Global Player Controls
	if (message == WM_KEYDOWN)
	{
		//Player Controls
		PrgAPI* pAPI = PRGAPI();
		MediaPlayer* pMP = pAPI->GetMediaPlayer();
		BOOL bHandled = TRUE;
		switch (wParam)
		{
		case VK_NUMPAD4:
			{
				DOUBLE pos = pMP->GetMediaPos();
				if (pos > 5.0)
					pMP->SetMediaPos(pos - 5.0);
			}
			break;
		case VK_NUMPAD6:
			{
				DOUBLE pos = pMP->GetMediaPos();
				DOUBLE len = pMP->GetMediaLength();
				if (pos < len - 5.0)
					pMP->SetMediaPos(pos + 5.0);
			}
			break;
		case VK_NUMPAD2:
			pMP->Next();
			break;
		case VK_NUMPAD8:
			pMP->Previous();
			break;
		case VK_NUMPAD0:
			pMP->Stop();
			break;
		case VK_NUMPAD5:
			if (pMP->GetPlayState() == PS_Playing)
				pMP->Pause();
			else
				pMP->Start();
			break;
		case VK_ADD:
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				pAPI->GetSkinManager()->IncreaseRelativeFontSize();
			}
			else
			{
				INT vol = pMP->GetVolume();
				vol += 5;
				if (vol > 100)
					vol = 100;
				pMP->SetVolume(vol);
				pAPI->SendMessage(SM_PlayerVolumeChanged);

			}
			break;
		case VK_SUBTRACT:
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				//Decrease Font Size
				pAPI->GetSkinManager()->DecreaseRelativeFontSize();
			}
			else
			{
				INT vol = pMP->GetVolume();
				vol -= 5;
				if (vol < 0)
					vol = 0;
				pMP->SetVolume(vol);
				pAPI->SendMessage(SM_PlayerVolumeChanged);
			}
			break;
		case VK_MULTIPLY:
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				pAPI->GetSkinManager()->ResetRelativeFontSize();
			}
			break;
		//case VK_ESCAPE:
		//	break;
		default:
			bHandled = FALSE;
		}
		if (bHandled)
			return AR_ReturnTrue;
	}
	//if (message == WM_CHAR)
	//{
	//	INT a = 0;
	//	switch (wParam)
	//	{
	//	case VK_NUMPAD0:
	//		a = 1; break;
	//	case VK_NUMPAD2:
	//		a = 1; break;
	//	case VK_NUMPAD4:
	//		a = 1; break;
	//	case VK_NUMPAD5:
	//		a = 1; break;
	//	case VK_NUMPAD6:
	//		a = 1; break;
	//	case VK_NUMPAD8:
	//		a = 1; break;
	//	case VK_ADD:
	//		a = 1; break;
	//	case VK_SUBTRACT:
	//		a = 1; break;
	//		return AR_ReturnTrue;
	//		break;
	//	}

	//}


	//...More handlers here

	return AR_DoNothing;
}
