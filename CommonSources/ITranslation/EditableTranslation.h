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
#ifndef _EDITABLETRANSLATION_H_
#define _EDITABLETRANSLATION_H_

#include "ITranslation.h"
#include <string>
#include <map>

class EditableTranslation: public ITranslation
{
public:
	EditableTranslation()									{}
	virtual ~EditableTranslation()							{}
	virtual LPCTSTR GetTranslationInfo(TranslationInfoEnum infoType)
	{
		ASSERT(infoType < TI_Last);
		if (infoType < TI_Last)
		{
			if (!m_info[infoType].empty())
				return m_info[infoType].c_str();
		}
		return NULL;
	}

	virtual LPCTSTR GetString(UINT stringID)
	{
		StringMap::iterator it = m_strings.find(stringID);
		if (it != m_strings.end())
			return it->second.c_str();
		return NULL;
	}

	//=== Extra Interface
	
	void SetString(UINT stringID, LPCTSTR value)
	{
		m_strings[stringID] = value;
	}
	void SetString(UINT stringID, LPCTSTR value, UINT length)
	{
#ifdef _DEBUG
		if (stringID == 434)
		{
			if (_tcsncmp(value, _T("Music browser"), 13) != 0)
			{
				INT a = 0;
			}
		}
#endif
		m_strings[stringID].insert(0, value, length);
	}
	void SetTranslationInfo(TranslationInfoEnum infoType, LPCTSTR value)
	{
		ASSERT(infoType < TI_Last);
		if (infoType < TI_Last)
			m_info[infoType] = value;
	}
	void SetTranslationInfo(TranslationInfoEnum infoType, LPCTSTR value, UINT length)
	{
		ASSERT(infoType < TI_Last);
		if (infoType < TI_Last)
		{
			m_info[infoType].clear();
			m_info[infoType].insert(0, value, length);
		}
	}
	void ClearStrings()
	{
		m_strings.clear();
	}
	UINT GetStringCount()
	{
		return m_strings.size();
	}


private:
	typedef std::map<UINT, std::basic_string<TCHAR> > StringMap;
	StringMap m_strings;
	std::basic_string<TCHAR> m_info[ITranslation::TI_Last];

};


#endif
