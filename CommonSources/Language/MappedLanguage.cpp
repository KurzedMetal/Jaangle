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
#include "MappedLanguage.h"

LPCTSTR MappedLanguage::GetString(UINT ResourceNumber)
{
	std::map<int, std::tstring>::const_iterator it = m_mapStrings.find(ResourceNumber);
	if (it != m_mapStrings.end())
		return (*it).second.c_str();
	return NULL;
}

LanguageInfo& MappedLanguage::GetLanguageInfo()
{
	return m_info;
}

void MappedLanguage::SetString(UINT ResourceNumber, LPCTSTR str)
{
	ASSERT(str != NULL);
	m_mapStrings[ResourceNumber] = str;
}
void MappedLanguage::ClearStrings()
{
	m_mapStrings.clear();
}

