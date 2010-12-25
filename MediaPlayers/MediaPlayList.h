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
#ifndef _MediaPlayList_h_
#define _MediaPlayList_h_


enum MediaPlayListItemFlags
{
	MPIF_None = 0,
	MPIF_CannotPlay = 1,
	MPIF_TagsRead = 2,
	MPIF_Last
};
struct MediaPlayListItem
{
	MediaPlayListItem(LPCTSTR Url)
	{
		memset(this, 0, sizeof(MediaPlayListItem));
		url = Url;
	}

	MediaPlayListItem()
	{
		memset(this, 0, sizeof(MediaPlayListItem));
	}
	LPCTSTR url;
	LPCTSTR artist;
	LPCTSTR title;
	UINT length;
	UINT rating;
	DWORD flags;
	BOOL IsValid()const{return url != NULL && url[0] != 0;}
};

class MediaPlayList
{
public:
	MediaPlayList()																{}
	virtual ~MediaPlayList()													{}

	virtual void Clear()														= 0;
	virtual INT GetCount()														= 0;
	virtual BOOL RemovePlayListItem(INT index)									= 0;
	virtual BOOL SwapPlayListItems(INT index1, INT index2)						= 0;
	virtual BOOL InsertPlayListItem(const MediaPlayListItem& mpli, INT index)	= 0;
	virtual BOOL UpdatePlayListItem(const MediaPlayListItem& mpli, INT index)	= 0;

	virtual BOOL GetPlayListItem(MediaPlayListItem& mpli, INT index)			= 0;
};


#endif
