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
#include "VectorPlaylist.h"

void VectorPlayList::Clear()
{
	m_storage.clear();
}

INT VectorPlayList::GetCount()
{
	return m_storage.size();
}

BOOL VectorPlayList::RemovePlayListItem(INT index)
{
	ASSERT(index >= 0);
	ASSERT(index < (INT)GetCount());
	if (index < (INT)GetCount())
	{
		m_storage.erase(m_storage.begin() + index);
		return TRUE;
	}
	return FALSE;
}

BOOL VectorPlayList::SwapPlayListItems(INT index1, INT index2)
{
	ASSERT(index1 < (INT)GetCount() && index2 < (INT)GetCount());
	if (index1 >= 0 && index2 >= 0 && index1 < (INT)GetCount() && index2 < (INT)GetCount())
	{
		PLItemSP it = m_storage[index1];
		m_storage[index1] = m_storage[index2];
		m_storage[index2] = it;
		return TRUE;
	}
	return FALSE;
}

BOOL VectorPlayList::InsertPlayListItem(const MediaPlayListItem& mpli, INT Pos)
{
	INT count = GetCount();
	if (Pos >= 0 && Pos < count)
		m_storage.insert(m_storage.begin() + Pos, PLItemSP(new PLItem(mpli)));
	else
		m_storage.push_back(PLItemSP(new PLItem(mpli)));
	return TRUE;
}

BOOL VectorPlayList::UpdatePlayListItem(const MediaPlayListItem& mpli, INT Pos)
{
	INT count = GetCount();
	if (Pos >= 0 && Pos < count)
	{
		*m_storage[Pos] = mpli;
		return TRUE;
	}
	return FALSE;;
}


BOOL VectorPlayList::GetPlayListItem(MediaPlayListItem& mpli, INT index)
{
	ASSERT(index >= 0 && index < (INT)GetCount());
	if (index >= 0 && index < (INT)GetCount())
	{
		PLItemSP& it = m_storage[index];
		mpli.artist = it->artist.c_str();
		mpli.title = it->title.c_str();
		mpli.url = it->url.c_str();
		mpli.length = it->length;
		mpli.rating = it->rating;
		mpli.flags = it->flags;
		return TRUE;
	}
	return FALSE;
}



