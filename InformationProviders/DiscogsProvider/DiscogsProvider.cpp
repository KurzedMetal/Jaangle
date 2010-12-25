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
#include "CompatibleWebInfoProvider.h"
#include "../InfoProviderHelper.h"

#ifdef _USRDLL	//----------------------------------------------------------------

CompatibleWebInfoProvider inst;
//EXTERNAL FUNCTIONS
extern "C" __declspec(dllexport) IInfoProvider* GetInfoProviderInst()
{
	return &inst;
}
#include "..\NetUtilities.h"

#else	//----------------------------------------------------------------

#include "..\..\PluginTestB.h"
#define GetPage CPluginTestBApp::GetPage
#include <string>

#endif	//----------------------------------------------------------------


CompatibleWebInfoProvider::CompatibleWebInfoProvider():
m_pResult(NULL),
m_params(NULL),
m_CurAttempt(0)
{

}

CompatibleWebInfoProvider::~CompatibleWebInfoProvider()
{
	delete m_pResult;
	delete m_params;
}

LPCTSTR CompatibleWebInfoProvider::Request(InfoItemType type, LPCTSTR params)
{
	ASSERT(params != NULL);
	if (params == NULL)
		return NULL;
	m_type = type;
	delete m_params;
	m_params = new TCHAR[_tcslen(params) + 1];
	_tcscpy(m_params, params);
	m_CurAttempt = 0;
	return RequestNext();
}

LPCTSTR CompatibleWebInfoProvider::GetLastInfoSource()
{
	return _T("http://www.theconium.com");
}


LPCTSTR CompatibleWebInfoProvider::RequestNext()
{
	m_CurAttempt++;
	delete m_pResult;
	m_pResult = NULL;
	if (m_params == NULL)
		return NULL;
	if (m_CurAttempt > 1)
		return NULL;//Only one site at this point

	LPCTSTR sType = NULL;
	LPTSTR artist = NULL, album = NULL, track = NULL;
	LPCTSTR mainURL = _T("http://www.theconium.com/infoservice.php");
	TCHAR Query[400];
	_sntprintf(Query, 400, _T("http://www.theconium.com/infoservice.php?q=%s&p=%s"), sType, m_params);

	switch (m_type)
	{
	case IIT_ArtistAll:
	case IIT_ArtistBio:
	case IIT_ArtistPicture:
		_sntprintf(Query, 400, _T("%s?q=artistall&artist=%s"), mainURL, m_params);
		break;
	case IIT_AlbumAll:
	case IIT_AlbumReview:
	case IIT_AlbumInfo:
	case IIT_AlbumPicture:
		IInfoProviderHelper::Params2AlbumInfo(m_params, artist, album);
		if (artist == NULL || album == NULL)
			return NULL;
		_sntprintf(Query, 400, _T("%s?q=albumall&artist=%s&album=%s"), mainURL, artist, album);
		delete artist;
		delete album;
		break;
	case IIT_TrackLyrics:
		IInfoProviderHelper::Params2TrackInfo(m_params, artist, track);
		if (artist == NULL || track == NULL)
			return NULL;
		_sntprintf(Query, 400, _T("%s?q=tracklyrics&artist=%s&track=%s"), mainURL, artist, album);
		delete artist;
		delete track;
		break;
	default:
		return NULL;
	}
	LPSTR resPage = GetPage(Query);
	if (resPage == NULL)
		return NULL;
	//Need to check the page for a format 
	//[OK]|...info... or [ERROR]|...reason....
	//or [OK]|[InfoType]...info...|...(continue like that)
	//kai isos na metakinithei pros sto Request afto pou kano edo kai to "RequestNext" na mou dinei tin
	//epomeni pliroforia
	TCHAR* res = NULL;
	UINT wLen = MultiByteToWideChar(CP_UTF8, 0, resPage, -1, NULL, 0);
	if (wLen > 0)
	{
		res = new TCHAR[wLen + 1];
		MultiByteToWideChar(CP_UTF8, 0, resPage, -1, res, wLen + 1);
	}
	return res;
}

LPCTSTR CompatibleWebInfoProvider::GetIdentity()
{
	return _T("Compatible Web Service|Alex Economou|1.0|1|Downloads info from compatible web sites|http://www.theconium.com|alex@theconium.com");
}

