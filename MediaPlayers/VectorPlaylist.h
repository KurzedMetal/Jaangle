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
#ifndef _VectorPlayList_h_
#define _VectorPlayList_h_

#include "MediaPlayList.h"
#include <vector>
#include <boost/shared_ptr.hpp>

class VectorPlayList:public MediaPlayList
{
public:
	VectorPlayList()												{}
	virtual ~VectorPlayList()										{}

	virtual void Clear();
	virtual INT GetCount();
	virtual BOOL RemovePlayListItem(INT index);
	virtual BOOL SwapPlayListItems(INT index1, INT index2);
	virtual BOOL InsertPlayListItem(const MediaPlayListItem& mpli, INT Pos);
	virtual BOOL UpdatePlayListItem(const MediaPlayListItem& mpli, INT index);

	virtual BOOL GetPlayListItem(MediaPlayListItem& mpli, INT index);

private:

	struct PLItem
	{
		PLItem(const MediaPlayListItem& mpli):
				artist(mpli.artist == NULL ? _T("") : mpli.artist),
				title(mpli.title == NULL ? _T("") : mpli.title),
				url(mpli.url == NULL ? _T("") : mpli.url),
				length(mpli.length),
				rating(mpli.rating),
				flags(mpli.flags)
			{

			}
		PLItem():length(0),rating(0),flags(0){}
		std::basic_string<TCHAR> artist;
		std::basic_string<TCHAR> title;
		std::basic_string<TCHAR> url;
		UINT length;
		UINT rating;
		DWORD flags;
	};
	typedef boost::shared_ptr<PLItem> PLItemSP;
	std::vector<PLItemSP> m_storage;
};

#endif
