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
#ifndef _ISKIN_H
#define _ISKIN_H

class ISkin
{
public:
	enum SkinInfoEnum
	{
		SI_DirName,
		SI_Name,
		SI_Author,
		SI_Version,
		SI_Email,
		SI_Description,
		SI_WebSite,
		SI_AutoUpdateURL,
		SI_Last
	};

public:
	virtual ~ISkin()		{}
	virtual LPCTSTR GetSkinInfo(SkinInfoEnum infoType)		= 0;

};

class ISkinnable
{
public:
	virtual ~ISkinnable()						{}
	virtual void ApplySkin(ISkin& skin)			= 0;
};

class ISkinManager
{
public:
	virtual ~ISkinManager()									{}
	virtual void RegisterSkinnable(ISkinnable& skinnable)	= 0;
	virtual void UnRegisterSkinnable(ISkinnable& skinnable)	= 0;
	virtual void UpdateSkinnables(ISkin& skin)				= 0;
};

#endif
