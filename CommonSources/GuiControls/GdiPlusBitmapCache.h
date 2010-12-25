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

#include "GdiPlusPicDrawer.h"

using namespace Gdiplus;

class GdiPlusBitmapCache
{
public:
	GdiPlusBitmapCache(UINT cx, UINT cy, UINT capacity);
	~GdiPlusBitmapCache();

public:

	//=== You have one large bitm
	BOOL LoadState(LPCTSTR stateFile);
	BOOL SaveState(LPCTSTR stateFile);
	UINT GetItemWidth()							{return m_cx;}
	UINT GetItemHeight()						{return m_cy;}
	UINT GetCapacity()							{return m_capacity;}

	BOOL Draw(DWORD uid, Graphics& g, Rect& rcTarget);
	BOOL DrawItem(INT nItem, Graphics& g, Rect& rcTarget);
	//=== Return then index or -1
	INT SetByPath(DWORD uid, LPCTSTR imagePath);

private:
	INT m_cx, m_cy, m_capacity;
	GdiPlusPicDrawer m_picDrawer;
	struct CacheItemInfo
	{
		CacheItemInfo():uid(0),lastTick(0){}
		DWORD uid;
		DWORD lastTick;
	};
	CacheItemInfo* m_items;
	INT Find(DWORD uid);
	INT FindOldest();
	struct Stats
	{
		Stats():Drawn(0),Loaded(0)	{}
		INT Drawn;
		INT Loaded;
	} m_stats;

};



