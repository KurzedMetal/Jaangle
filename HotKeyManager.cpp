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
#include "HotKeyManager.h"
#include "PrgAPI.h"
#include "AppSettings/AppSettings.h"

HotKeyManager::HotKeyManager():
m_hWnd(NULL),
m_bMultimediaHotKeysEnabled(FALSE)
{
}
HotKeyManager::~HotKeyManager()
{
	SetHotKey(HK_Play, 0, 0);
	SetHotKey(HK_Stop, 0, 0);
	SetHotKey(HK_Prev, 0, 0);
	SetHotKey(HK_Next, 0, 0);
	SetHotKey(HK_ToggleMiniPlayer, 0, 0);
	EnableMultimediaHotKeys(FALSE);
}

LPCTSTR HotKeyManagerSettingsCategory = _T("HotKeys");
LPCTSTR cHotkeyPlay = _T("HotkeyPlay");
LPCTSTR cHotkeyStop = _T("HotkeyStop");
LPCTSTR cHotkeyPrev = _T("HotkeyPrev");
LPCTSTR cHotkeyNext = _T("HotkeyNext");
LPCTSTR cHotkeyMiniPlayer = _T("HotkeyMiniPlayer");
LPCTSTR cHotkeyTooltip = _T("HotkeyTooltip");
LPCTSTR cHotkeyQuickSearch = _T("HotkeyQuickSearch");


void HotKeyManager::Init()//HWND hWndTarget)
{
	PrgAPI* pAPI = PRGAPI();
	AppSettings* appSettings = pAPI->GetAppSettings();
	INT value = 0;
	m_hWnd = NULL;//hWndTarget;

	appSettings->Read(HotKeyManagerSettingsCategory, cHotkeyPlay, value, 0);
	SetHotKey(HK_Play, HIWORD(value), LOWORD(value));
	appSettings->Read(HotKeyManagerSettingsCategory, cHotkeyStop, value, 0);
	SetHotKey(HK_Stop, HIWORD(value), LOWORD(value));
	appSettings->Read(HotKeyManagerSettingsCategory, cHotkeyPrev, value, 0);
	SetHotKey(HK_Prev, HIWORD(value), LOWORD(value));
	appSettings->Read(HotKeyManagerSettingsCategory, cHotkeyNext, value, 0);
	SetHotKey(HK_Next, HIWORD(value), LOWORD(value));
	appSettings->Read(HotKeyManagerSettingsCategory, cHotkeyMiniPlayer, value, 0);
	SetHotKey(HK_ToggleMiniPlayer, HIWORD(value), LOWORD(value));
	appSettings->Read(HotKeyManagerSettingsCategory, cHotkeyTooltip, value, 0);
	SetHotKey(HK_Tooltip, HIWORD(value), LOWORD(value));
	appSettings->Read(HotKeyManagerSettingsCategory, cHotkeyQuickSearch, value, 0);
	SetHotKey(HK_QuickSearch, HIWORD(value), LOWORD(value));


	EnableMultimediaHotKeys(pAPI->GetOption(OPT_GEN_MultimediaKeysActive) == 1);

}



void HotKeyManager::EnableMultimediaHotKeys(BOOL bEnable)
{
	//ASSERT(m_hWnd != NULL);
	if (m_bMultimediaHotKeysEnabled != bEnable)// && m_hWnd != NULL)
	{
		m_bMultimediaHotKeysEnabled = bEnable;
		if (bEnable)
		{
			RegisterHK(VK_MEDIA_NEXT_TRACK, NULL, VK_MEDIA_NEXT_TRACK);
			RegisterHK(VK_MEDIA_PREV_TRACK, NULL, VK_MEDIA_PREV_TRACK);
			RegisterHK(VK_MEDIA_STOP, NULL, VK_MEDIA_STOP);
			RegisterHK(VK_MEDIA_PLAY_PAUSE, NULL, VK_MEDIA_PLAY_PAUSE);
		}
		else
		{
			UnregisterHotKey(m_hWnd, VK_MEDIA_NEXT_TRACK);
			UnregisterHotKey(m_hWnd, VK_MEDIA_PREV_TRACK);
			UnregisterHotKey(m_hWnd, VK_MEDIA_STOP);
			UnregisterHotKey(m_hWnd, VK_MEDIA_PLAY_PAUSE);
		}
	}
}

BOOL HotKeyManager::RegisterHK(INT id, UINT fsModifiers, UINT vk)
{
	BOOL bRet = RegisterHotKey(m_hWnd, id, fsModifiers, vk);
	if (!bRet)
	{
		INT err = GetLastError();
		TRACE(_T("@1@E HotKeyManager::RegisterHK. Cannot register: %d,%d\r\n"), id, vk);
		return FALSE;
	}
	return TRUE;
}

void HotKeyManager::SaveSettings()
{
	TRACEST(_T("HotKeyManager::SaveSettings."));
	PrgAPI* pAPI = PRGAPI();
	AppSettings* appSettings = pAPI->GetAppSettings();
	appSettings->Write(HotKeyManagerSettingsCategory, cHotkeyPlay, (INT) m_HotKeys[HK_Play - HK_First - 1]);
	appSettings->Write(HotKeyManagerSettingsCategory, cHotkeyStop, (INT) m_HotKeys[HK_Stop - HK_First - 1]);
	appSettings->Write(HotKeyManagerSettingsCategory, cHotkeyPrev, (INT) m_HotKeys[HK_Prev - HK_First - 1]);
	appSettings->Write(HotKeyManagerSettingsCategory, cHotkeyNext, (INT) m_HotKeys[HK_Next - HK_First - 1]);
	appSettings->Write(HotKeyManagerSettingsCategory, cHotkeyMiniPlayer, (INT) m_HotKeys[HK_ToggleMiniPlayer - HK_First - 1]);
	appSettings->Write(HotKeyManagerSettingsCategory, cHotkeyTooltip, (INT) m_HotKeys[HK_Tooltip - HK_First - 1]);
	appSettings->Write(HotKeyManagerSettingsCategory, cHotkeyQuickSearch, (INT) m_HotKeys[HK_QuickSearch - HK_First - 1]);

}

void HotKeyManager::GetHotKey(HotKeysEnum hk, WORD &vk, WORD &mod)
{
	ASSERT(hk > HK_First && hk < HK_Last);
	DWORD h = m_HotKeys[hk - HK_First - 1];
	vk = HIWORD(h);
	mod = LOWORD(h);
}

void HotKeyManager::SetHotKey(HotKeysEnum hk, WORD vk, WORD mod)
{
	ASSERT(hk > HK_First && hk < HK_Last);
	DWORD newHkVal = MAKEWPARAM(mod, vk);
	DWORD oldHkVal = m_HotKeys[hk - HK_First - 1];
	if (newHkVal != oldHkVal)
	{
		if (oldHkVal != 0)
			::UnregisterHotKey(m_hWnd, hk);
		if (newHkVal != 0)
		{
			if (::RegisterHotKey(m_hWnd, hk, mod, vk))
				m_HotKeys[hk - HK_First - 1] = newHkVal;
			else
			{
				TRACE(_T("@1 CMediaPlayerCtrl::SetHotKey. Failed\r\n"));
				m_HotKeys[hk - HK_First - 1] = 0;
			}
		}
		else
			m_HotKeys[hk - HK_First - 1] = 0;
	}

}
