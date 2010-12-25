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

//CompatibleWebInfoProvider inst;
////EXTERNAL FUNCTIONS
//extern "C" __declspec(dllexport) IInfoProvider* GetInfoProviderInst()
//{
//	return &inst;
//}
extern "C" __declspec(dllexport) IInfoProvider* CreateInfoProvider()
{
	return new CompatibleWebInfoProvider;
}
extern "C" __declspec(dllexport) void DeleteInfoProvider(IInfoProvider* pIP)
{
	delete pIP;
}
#include "..\NetUtilities.h"

#else	//----------------------------------------------------------------

#include "..\..\PluginTestB.h"
#define GetPage CPluginTestBApp::GetPage
#include <string>

#endif	//----------------------------------------------------------------

CompatibleWebInfoProvider::CompatibleWebInfoProvider()
{
}

CompatibleWebInfoProvider::~CompatibleWebInfoProvider()
{
}


BOOL CompatibleWebInfoProvider::Request(InfoItemType type, LPCTSTR params)
{
	ASSERT(params != NULL);
	if (params == NULL)
		return FALSE;
	m_type = type;
	m_params = params;
	return TRUE;
}


BOOL CompatibleWebInfoProvider::GetResult(InfoProviderResult* pIPR, INT idx)
{
	return FALSE;

	LPCTSTR sType = NULL;
	LPCTSTR mainURL = _T("http://www.theconium.com/infoservice.php");
	TCHAR Query[400];
	Query[0] = 0;

	switch (m_type)
	{
	case IIT_ArtistBio:
	case IIT_ArtistPicture:
		_sntprintf(Query, 400, _T("%s?q=artistall&artist=%s"), mainURL, m_params);
		break;
	case IIT_AlbumReview:
	case IIT_AlbumInfo:
	case IIT_AlbumPicture:
		{
			TCHAR artist[100];
			TCHAR album[100];
			if (IInfoProviderHelper::Params2AlbumInfo(m_params.c_str(), artist, 100, album, 100))
				_sntprintf(Query, 400, _T("%s?q=albumall&artist=%s&album=%s"), mainURL, artist, album);
		}
		break;
	case IIT_TrackLyrics:
		{
			TCHAR artist[100];
			TCHAR track[100];
			if (IInfoProviderHelper::Params2TrackInfo(m_params.c_str(), artist, 100, track, 100))
				_sntprintf(Query, 400, _T("%s?q=albumall&artist=%s&album=%s"), mainURL, artist, track);
		}
		break;
	default:
		ASSERT(0);
	}
	if (Query[0] == 0)
		return NULL;
	LPSTR resPage = GetPage(Query);
	if (resPage == NULL)
		return NULL;
	//Need to check the page for a format 
	//[OK]|...info... or [ERROR]|...reason....
	//or [OK]|[InfoType]...info...|...(continue like that)
	//kai isos na metakinithei pros sto Request afto pou kano edo kai to "RequestNext" na mou dinei tin
	//epomeni pliroforia
	
	//.....Thelei doulia edo
	ASSERT(0);
	return NULL;
	TCHAR* res = NULL;
	UINT wLen = MultiByteToWideChar(CP_UTF8, 0, resPage, -1, NULL, 0);
	if (wLen > 0)
	{
		res = new TCHAR[wLen + 1];
		MultiByteToWideChar(CP_UTF8, 0, resPage, -1, res, wLen + 1);
	}
	m_Result = res;
	m_type = IIT_AlbumInfo;
	return FALSE;
}

LPCTSTR CompatibleWebInfoProvider::GetModuleInfo(ModuleInfo mi)
{
	switch (mi)
	{
	case IPI_UniqueID:		return _T("COMP");
	case IPI_Name:			return _T("Compatible Web Service");
	case IPI_Author:		return _T("Alex Economou");
	case IPI_VersionStr:	return _T("1.0");
	case IPI_VersionNum:	return _T("1");
	case IPI_Description:	return _T("Downloads info from compatible web sites");
	case IPI_HomePage:		return _T("http://www.theconium.com");
	case IPI_Email:			return _T("alex@theconium.com");
	}
	return IInfoProvider::GetModuleInfo(mi);
}

