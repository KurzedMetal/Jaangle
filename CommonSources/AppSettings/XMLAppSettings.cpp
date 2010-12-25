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
#include "XMLAppSettings.h"
#include "XmlNodeWrapper.h"



XMLAppSettings::XMLAppSettings()
{
	m_Path[0] = 0;
}

XMLAppSettings::XMLAppSettings(LPCTSTR connectionString)
{
	ASSERT(connectionString != NULL);
	_tcsncpy(m_Path, connectionString, MAX_PATH);
}

XMLAppSettings::~XMLAppSettings()
{
	Save();
}

BOOL XMLAppSettings::Load(LPCTSTR connectionString)
{
	if (connectionString != NULL)
		_tcsncpy(m_Path, connectionString, MAX_PATH);
	if (m_Path[0] == 0)
		GetDefaultFileName(m_Path, MAX_PATH);

	CXmlDocumentWrapper xml;
	if (xml.Load(m_Path))
	{
		CXmlNodeWrapper root(xml.AsNode());
		for (int i = 0; i < root.NumNodes(); i++)
		{
			CXmlNodeWrapper attribs(root.GetNode(i));
			TRACE(_T("@2---Category: %s\r\n"), attribs.Name());
			for (int j = 0; j < attribs.NumAttributes(); j++)
			{
				m_settings[attribs.Name()][attribs.GetAttribName(j)] = attribs.GetAttribVal(j);
			}
		}
	}
	return FALSE;

}

BOOL XMLAppSettings::Save()
{
	if (m_Path[0] == 0)
		return FALSE;
	CXmlDocumentWrapper xml;
	xml.LoadXML(_T("<OPTIONS></OPTIONS>"));
	std::map<CString, std::map<CString, CString> >::iterator i = m_settings.begin();
	CXmlNodeWrapper root(xml.AsNode());
	for (; i != m_settings.end(); i++)
	{
		TRACE(_T("@2 - Categ: %s\r\n"), (*i).first);
		std::map<CString, CString>::iterator j = (*i).second.begin();
		CXmlNodeWrapper cat(root.InsertNode(root.NumNodes(), (*i).first));

		for (; j != (*i).second.end(); j++)
		{
			cat.SetValue((*j).first, (*j).second);
			//TRACE(_T("@2 - %s:%s=%s\r\n"), (*i).first, (*j).first, (*j).second);
		}
	}
	//CXmlNodeWrapper root(xml.AsNode());
	//root.
	return xml.Save(m_Path);
}

BOOL XMLAppSettings::Read(LPCTSTR category, LPCTSTR setting, INT &value, INT defaultVal)
{
	std::tstring val;
	if (Read(category, setting, val, _T("")))
	{
		if (val.size() > 0)
			value = _ttoi(val.c_str());
		else
			value = defaultVal;
		return TRUE; 
	}
	return FALSE;
}

BOOL XMLAppSettings::Read(LPCTSTR category, LPCTSTR setting, std::tstring &value, LPCTSTR defaultVal)
{
	ASSERT(category != NULL);
	ASSERT(setting!=NULL);
	value = m_settings[category][setting];
	if (value.empty())
		value = defaultVal;
	return TRUE;
}

BOOL XMLAppSettings::Write(LPCTSTR category, LPCTSTR setting, INT value)
{
	TCHAR bf[30];
	_itot(value, bf, 10);
	return Write(category, setting, bf);
}

BOOL XMLAppSettings::Write(LPCTSTR category, LPCTSTR setting, LPCTSTR value)
{
	ASSERT(category != NULL);
	ASSERT(setting != NULL);
	m_settings[category][setting] = value;
	return TRUE;
}

void XMLAppSettings::GetDefaultFileName(LPTSTR DefaultFileName, UINT BufferLen)
{
	ASSERT(DefaultFileName != NULL);
	ASSERT(BufferLen >= MAX_PATH);
	VERIFY(::GetModuleFileName(AfxGetInstanceHandle(), DefaultFileName, BufferLen));
	TCHAR *ch = _tcsrchr(DefaultFileName,'.');
	ch++;
	*ch = 0;
	_tcscat(DefaultFileName, _T("xml"));
}


