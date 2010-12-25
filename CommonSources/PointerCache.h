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
#ifndef _PointerCache_h_
#define _PointerCache_h_

#include <map>

template <class T> class PointerCache 
{
private:
	struct CacheItem
	{
		CacheItem():item(NULL),lastUseTick(0){}
		T* item;
		DWORD lastUseTick;
	};

public:
	PointerCache(INT storageSize):m_storageSize(storageSize)	{}
	~PointerCache()												{CleanUp();}

	T* Get(DWORD id)
	{
		std::map<DWORD, CacheItem>::iterator it = m_storage.find(id);
		if (it == m_storage.end())
			return NULL;
		CacheItem& cit = it->second;
		cit.lastUseTick = GetTickCount();
		return cit.item;
	}
	void Set(DWORD id, T* item)
	{
		std::map<DWORD, CacheItem>::iterator it = m_storage.find(id);
		if (it != m_storage.end())
		{
			CacheItem& cit = it->second;
			cit.lastUseTick = GetTickCount();
			delete cit.item;
			cit.item = item;
		}
		else
		{
			if (m_storage.size() >= m_storageSize)
				RemoveItem();
			CacheItem it;
			it.lastUseTick = GetTickCount();
			it.item = item;
			m_storage[id] = it;
		}

	}

private:
	void CleanUp()
	{
		std::map<DWORD, CacheItem>::iterator it = m_storage.begin();
		UINT smallestTick = 0xffffffff;
		CacheItem* smallestCI = NULL;
		for (;it != m_storage.end(); it++)
		{
			CacheItem& ci = it->second;
			delete ci.item;
		}
		m_storage.clear();
	}

	void RemoveItem()
	{
		std::map<DWORD, CacheItem>::iterator it = m_storage.begin();
		UINT smallestTick = 0xffffffff;
		std::map<DWORD, CacheItem>::iterator foundItem = it;
		for (;it != m_storage.end(); it++)
		{
			CacheItem& ci = it->second;
			if (ci.lastUseTick < smallestTick)
			{
				smallestTick = ci.lastUseTick;
				foundItem = it;
			}
		}
		delete foundItem->second.item;
		m_storage.erase(foundItem);
	}
private:
	std::map<DWORD, CacheItem> m_storage;
	DWORD m_storageSize;
};


#endif
