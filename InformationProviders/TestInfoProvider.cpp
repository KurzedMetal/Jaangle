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
#include "TestInfoProvider.h"
#include "InfoProviderHelper.h"

TestInfoProvider::TestInfoProvider():
m_pResult(NULL)
{
	
}

TestInfoProvider::~TestInfoProvider()
{
	delete m_pResult;
}


BOOL TestInfoProvider::Request(InfoItemType type, LPCTSTR params)
{
	return FALSE;
	/*
	LPTSTR Artist = NULL, Album = NULL;
	IInfoProviderHelper::Params2AlbumInfo(params, Artist, Album);
	LPSTR pg = GetPage(_T("http://www.choicexs.com"));
	m_pResult = new TCHAR(strlen(pg) + 1);
	INT len = MultiByteToWideChar(CP_THREAD_ACP, MB_COMPOSITE, pg, -1, NULL, NULL);
	if (len > 0)
	{
		m_pResult = new TCHAR[len + 1];
		MultiByteToWideChar(CP_THREAD_ACP, MB_COMPOSITE, pg, -1, m_pResult, len + 1);
		m_pResult[len] = 0;
	}
	free(pg);
	//m_pResult = new TCHAR [1000];
	//_sntprintf(m_pResult, 1000, _T("Result for Artist: '%s' Album: '%s'."), Artist, Album);
	delete Artist;
	delete Album;
	*/
}

BOOL TestInfoProvider::GetResult(InfoProviderResult* pIPR, INT idx)
{
	return FALSE;
}

LPCTSTR TestInfoProvider::GetModuleInfo(ModuleInfo mi)
{
	switch (mi)
	{
	case IPI_UniqueID:		return _T("TEST");
	case IPI_Name:			return _T("Test");
	case IPI_Author:		return _T("Alex Economou");
	case IPI_VersionStr:	return _T("1.0");
	case IPI_VersionNum:	return _T("1");
	case IPI_Description:	return _T("Test Description");
	case IPI_HomePage:		return _T("http://www.theconium.com");
	case IPI_Email:			return _T("alex@theconium.com");
	}
	return IInfoProvider::GetModuleInfo(mi);
}

