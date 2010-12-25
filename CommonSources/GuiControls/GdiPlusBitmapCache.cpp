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
#include "GdiPlusBitmapCache.h"

GdiPlusBitmapCache::GdiPlusBitmapCache(UINT cx, UINT cy, UINT capacity):
	  m_cx(cx),m_cy(cy),m_capacity(capacity)		
{
	m_items = new CacheItemInfo[capacity];
	GdiPlusPicDrawer::DrawParams& params = m_picDrawer.GetDrawParams();
	params.zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
}
GdiPlusBitmapCache::~GdiPlusBitmapCache()																	
{
	delete m_items;
}

BOOL GdiPlusBitmapCache::LoadState(LPCTSTR stateFile)
{
	if (m_picDrawer.LoadFile(stateFile))
	{
		CSize sz = m_picDrawer.GetPictureSize();
		CSize desiredSize;
		desiredSize.cx = m_cx;
		desiredSize.cy = m_cy * m_capacity;
		if (sz == desiredSize)
		{
			//=== Image dimensions are correct. Lets load the info now
			TCHAR bf[MAX_PATH];
			_sntprintf(bf, MAX_PATH, _T("%s.id"), stateFile);
			HANDLE h = CreateFile(bf, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
			if (h != INVALID_HANDLE_VALUE)
			{
				BOOL bRet = TRUE;
				for (INT i = 0; i < m_capacity; i++)
				{
					DWORD bytesRead;
					if (!ReadFile(h, &m_items[i].uid, sizeof(DWORD), &bytesRead, 0))
					{
						bRet = FALSE;
						break;
					}
				}
				CloseHandle(h);
				return bRet;
			}
		}
	}
	m_picDrawer.Destroy();
	return FALSE;
}
BOOL GdiPlusBitmapCache::SaveState(LPCTSTR stateFile)
{
	if (m_picDrawer.SaveFile(stateFile))
	{
		TCHAR bf[MAX_PATH];
		_sntprintf(bf, MAX_PATH, _T("%s.id"), stateFile);
		HANDLE h = CreateFile(bf, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
		if (h != INVALID_HANDLE_VALUE)
		{
			BOOL bRet = TRUE;
			for (INT i = 0; i < m_capacity; i++)
			{
				DWORD bytesRead;
				if (!WriteFile(h, &m_items[i].uid, sizeof(DWORD), &bytesRead, 0))
				{
					bRet = FALSE;
					break;
				}
			}
			CloseHandle(h);
			return bRet;
		}
	}
	return FALSE;
}
BOOL GdiPlusBitmapCache::Draw(DWORD uid, Graphics& g, Rect& rcTarget)
{
	INT nItem = Find(uid);
	if (nItem == -1)
		return FALSE;
	return DrawItem(nItem, g, rcTarget);		
}

BOOL GdiPlusBitmapCache::DrawItem(INT nItem, Graphics& g, Rect& rcTarget)
{
	ASSERT(m_picDrawer.HasImage());
	ASSERT(nItem >= 0 && nItem < m_capacity);
	if (!(nItem >= 0 && nItem < m_capacity))
		return FALSE;
	if (m_picDrawer.HasImage() == FALSE)
		return FALSE;
	//TRACE(_T("GdiPlusBitmapCache::DrawItem Cache hit! d/l %d/%d\r\n"), m_stats.Drawn, m_stats.Loaded);
	m_stats.Drawn++;
	m_items[nItem].lastTick = GetTickCount();
	Rect rcSource;
	rcSource.X = 0;
	rcSource.Width = m_cx;
	rcSource.Y = nItem * m_cy;
	rcSource.Height = m_cy;
	return m_picDrawer.Draw(g, rcTarget, &rcSource);
}

INT GdiPlusBitmapCache::SetByPath(DWORD uid, LPCTSTR imagePath)
{
	//TRACE(_T("GdiPlusBitmapCache::SetByPath Adding thumbnail...\r\n"));
	GdiPlusPicDrawer pic;
	pic.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
	pic.GetDrawParams().positionY = 0;
	if (pic.LoadFile(imagePath))
	{
		m_stats.Loaded++;
		INT nItem = Find(uid);
		if (nItem == -1)
			nItem = FindOldest();
		ASSERT(nItem >= 0);
		Rect rcTarget;
		rcTarget.X = 0;
		rcTarget.Width = m_cx;
		rcTarget.Y = nItem * m_cy;
		rcTarget.Height = m_cy;

		if (m_picDrawer.HasImage() == FALSE)
		{
			TRACE(_T("GdiPlusBitmapCache::PreparePicDrawer Creating %dx%d Cache bitmap\r\n"), m_cx, m_cy);
			if (m_picDrawer.Create(m_cx, m_cy * m_capacity) == FALSE)
				ASSERT(0);
		}
		if (pic.Draw(*m_picDrawer.GetInternalGraphics(), rcTarget) == FALSE)
			return FALSE;
		TRACE(_T("GdiPlusBitmapCache::SetByPath item: %d\r\n"), nItem);
		m_items[nItem].uid = uid;
		return nItem;
	}
	return -1;
}

INT GdiPlusBitmapCache::Find(DWORD uid)
{
	for (INT i = 0; i < m_capacity; i++)
	{
		if (m_items[i].uid == 0)
			return -1;
		if (m_items[i].uid == uid)
			return i;
	}
	return -1;
}
INT GdiPlusBitmapCache::FindOldest()
{
	INT oldestItem = 0;
	for (INT i = 0; i < m_capacity; i++)
	{
		if (m_items[i].lastTick == 0)
			return i;
		if (m_items[i].lastTick < m_items[oldestItem].lastTick)
			oldestItem = i;
	}
	return oldestItem;

}


