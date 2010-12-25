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
#ifndef _ILanguage_H
#define _ILanguage_H


struct LanguageInfo
{
	LanguageInfo():BuildNum(0),IsValid(FALSE)		{}
	std::tstring Name;
	std::tstring LocalizedName;
	std::tstring Author;
	std::tstring Email;
	std::tstring WebSite;
	std::tstring Description;
	std::tstring Version;
	std::tstring AutoUpdateURL;
	UINT BuildNum;
	BOOL IsValid;
};
//-----------------------------------------------Experimental Interface
//---ILanguage: 
//---A Language Instantiation. 
//---Get Info - Strings from these objects
class ILanguage
{
public:
	virtual ~ILanguage(){}
	virtual LPCTSTR GetString(UINT ResourceNumber)				= 0;
	virtual void SetString(UINT ResourceNumber, LPCTSTR str)	= 0;
	virtual void ClearStrings()									= 0;
	virtual UINT GetStringCount()								= 0;
	virtual LanguageInfo& GetLanguageInfo()						= 0;
};

#endif
