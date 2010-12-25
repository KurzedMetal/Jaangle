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
#ifndef _ITRANSLATION_H_
#define _ITRANSLATION_H_

class ITranslation
{
public:
	enum TranslationInfoEnum
	{
		TI_Name,
		TI_LocalizedName,
		TI_Author,
		TI_Email,
		TI_WebSite,
		TI_Description,
		TI_Version,
		TI_AutoUpdateURL,
		TI_AppVersion,
		TI_Last
	};

public:
	virtual ~ITranslation()												{}
	virtual LPCTSTR GetTranslationInfo(TranslationInfoEnum infoType)	= 0;
	virtual LPCTSTR GetString(UINT stringID)							= 0;
};

class ITranslatable
{
public:
	virtual ~ITranslatable()											{}
	virtual void ApplyTranslation(ITranslation& translation)			= 0;
};

class ITranslationManager
{
public:
	virtual ~ITranslationManager()										{}
	virtual void RegisterTranslatable(ITranslatable& translatable)		= 0;
	virtual void UnRegisterTranslatable(ITranslatable& translatable)	= 0;
	virtual void UpdateTranslatables(ITranslation& translation)			= 0;
};

#endif
