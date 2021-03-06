﻿//	/*
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
#ifndef _BassTagger_h_
#define _BassTagger_h_

#include "FileTagger.h"
#include "BassInstance.h"

class BassTagger: public FileTagger
{
public:
	BassTagger();
	virtual ~BassTagger();

	//In the best case it ll return: TagInfo_Length|TagInfo_FormatInfo
	virtual UINT Read(LPCTSTR fileName,	TagInfo& tagInfo);
	virtual UINT Write(LPCTSTR fileName, TagInfo& tagInfo)		{return TagInfo_None;}
	virtual BOOL ClearTags(LPCTSTR fileName)					{return FALSE;}

	virtual void SetConfig(Config conf, INT val)	{};
	virtual INT GetConfig(Config conf)				{return -1;};

private:
	BassInstance m_bassInstance;
}; 


#endif
