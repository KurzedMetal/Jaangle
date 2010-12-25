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
#ifndef _HotKeyManager_h_
#define _HotKeyManager_h_

class HotKeyManager
{
public:
	enum HotKeysEnum
	{
		HK_First = 100,
		HK_Play,
		HK_Stop,
		HK_Prev,
		HK_Next,
		HK_ToggleMiniPlayer,
		HK_Tooltip,
		HK_QuickSearch,
		HK_Last
	};
public:
	HotKeyManager();
	~HotKeyManager();

public:
	void Init();//HWND hWndTarget);
	void SaveSettings();

	void EnableMultimediaHotKeys(BOOL bEnable);
	BOOL IsMultimediaHotKeysEnabled()					{return m_bMultimediaHotKeysEnabled;}
	void GetHotKey(HotKeysEnum hk, WORD& vk, WORD& mod);
	void SetHotKey(HotKeysEnum hk, WORD vk, WORD mod);

private:
	BOOL RegisterHK(INT id, UINT fsModifiers, UINT vk);

	BOOL m_bMultimediaHotKeysEnabled;
	HWND m_hWnd;
	DWORD m_HotKeys[HK_Last - HK_First - 1];
};

#endif
