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
#ifndef _FileTagger_h_
#define _FileTagger_h_

#include "TagInfo.h"

class FileTagger
{
public:
	FileTagger()															{}
	virtual ~FileTagger()													{}

	virtual UINT Read(LPCTSTR fileName,	TagInfo& tagInfo)					= 0;
	virtual UINT Write(LPCTSTR fileName, TagInfo& TagInfo)					= 0;
	virtual BOOL ClearTags(LPCTSTR fileName)								= 0;

	enum Config
	{
		CONF_ReadDepth,		//0=Safe - 100=UNSafe
		CONF_Last
	};
	virtual void SetConfig(Config conf, INT val)	= 0;
	virtual INT GetConfig(Config conf)				= 0;

}; 


#endif
