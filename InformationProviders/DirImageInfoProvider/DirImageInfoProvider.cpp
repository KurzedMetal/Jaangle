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
#include "DirImageInfoProvider.h"
#include "cStringUtils.h"



#ifdef _UNITTESTING

//#define UNITTESTNOSQLMANAGER
LPCTSTR GetFirstFileByArtistAlbumResult = NULL;
const int lastSizeOf = 120;

BOOL DirImageInfoProvider::UnitTest()
{
	if (lastSizeOf != sizeof(DirImageInfoProvider))
		TRACE(_T("TestDirImageInfoProvider. Object Size Changed. Was: %d - Is: %d\r\n"), lastSizeOf, sizeof(DirImageInfoProvider));

	DirImageInfoProvider ip;
#ifdef UNITTESTNOSQLMANAGER
	GetFirstFileByArtistAlbumResult = _T("D:\\Music\\Nick Cave\\1984 -  From Her To Eternity\\");
#endif 
	Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = _T("Nick Cave");
	req.album = _T("From Her To Eternity");
	if (ip.OpenRequest(req))
	{
		Result res;
		DWORD resNum = 0;
		while (ip.GetNextResult(res))
		{
			UNITTEST(res.IsValid());
			TRACE(_T("Result %d. %s\r\n"), resNum++, res.main);
		}
	}
#ifdef UNITTESTNOSQLMANAGER
	GetFirstFileByArtistAlbumResult = _T("D:\\Musi0\\90s\\sublime-second-hand_smoke-1997-jayms\\");
#endif 
	req.artist = _T("Sublime");
	req.album = _T("Second Hand Smoke");
	if (ip.OpenRequest(req))
	{
		Result res;
		DWORD resNum = 0;
		while (ip.GetNextResult(res))
		{
			UNITTEST(res.IsValid());
			TRACE(_T("Result %d. %s\r\n"), resNum++, res.main);
		}
	}
	return TRUE;
}
#endif

#ifndef UNITTESTNOSQLMANAGER
#include <SQLManager.h>
#endif

DirImageInfoProvider::DirImageInfoProvider():
m_pSQLManager(NULL),
m_curResult(-1),
m_StrictCriteriaMaxFilesNum(25),
m_StrictCriteriaMaxFilesSize(500000),
m_request(SRV_First)
{

}

DirImageInfoProvider::~DirImageInfoProvider()
{
}


IInfoProvider* DirImageInfoProvider::Clone() const
{
	DirImageInfoProvider* pIP = new DirImageInfoProvider;
	pIP->SetInternetHandle(GetInternetHandle());
	IConfigurableHelper::TransferConfiguration(*this, *pIP);
	pIP->SetSQLManager(m_pSQLManager);
	return pIP;
}

BOOL DirImageInfoProvider::OpenRequest(const Request& request)
{
	ASSERT(request.IsValid());
	if (!request.IsValid())
		return FALSE;
#ifndef UNITTESTNOSQLMANAGER
	ASSERT(m_pSQLManager != NULL);
	if (m_pSQLManager == NULL)
		return FALSE;
#endif
	if (request.service != SRV_AlbumImage)
		return FALSE;
	m_Artist = request.artist;
	m_Album = request.album;
	m_request.artist = m_Artist.c_str();
	m_request.album = m_Album.c_str();
	m_request.service = request.service;
	m_results.clear();
	m_curResult = -1;
	return TRUE;
}

BOOL DirImageInfoProvider::GetFirstFileByArtistAlbum(LPTSTR path, LPCTSTR artist, LPCTSTR album)
{
	ASSERT(path != NULL && artist != NULL && album != NULL);
#ifdef UNITTESTNOSQLMANAGER

	_tcscpy(path, GetFirstFileByArtistAlbumResult);
	return TRUE;
#else
	ASSERT(m_pSQLManager != NULL);
	if (m_pSQLManager == NULL)
		return FALSE;
	TracksFilter tf;
	tf.Album.val = album;
	tf.Album.match = TXTM_Exact;
	tf.Artist.val = artist;
	tf.Artist.match = TXTM_Exact;
	FullTrackRecordCollection col;
	if (m_pSQLManager->GetFullTrackRecordCollectionByTracksFilter(col, tf, 1))
	{
		if (col.size() > 0)
		{
			_tcsncpy(path, col[0]->track.location.c_str(), MAX_PATH);
			TCHAR* pos = _tcsrchr(path, '\\');
			if (pos != NULL)
			{
				pos++;
				*pos = 0;
				return TRUE;
			}
		}
	}
#endif
	return FALSE;
}

BOOL DirImageInfoProvider::GetNextResult(Result& result)
{
	if (m_curResult == -1)
	{
		TCHAR path[MAX_PATH];
		if (GetFirstFileByArtistAlbum(path, m_Artist.c_str(), m_Album.c_str()))
		{
			BOOL bStrictSearch = HasMoreThanXFilesWithMinSizeY(path, m_StrictCriteriaMaxFilesNum, m_StrictCriteriaMaxFilesSize);
			WIN32_FIND_DATA dta;
			TCHAR wildCard[MAX_PATH];
			_sntprintf(wildCard, MAX_PATH, _T("%s*.jpg"), path);
			HANDLE hFileFinder = FindFirstFile(wildCard, &dta);
			if (hFileFinder != INVALID_HANDLE_VALUE)
			{
				do 
				{
					TCHAR bf[MAX_PATH];
					_sntprintf(bf, MAX_PATH, _T("%s%s"), path, dta.cFileName);
					if (IsLegalPicture(bf, m_Album.c_str(), bStrictSearch))
					{
						m_results.push_back(bf);
					}
				} 
				while (FindNextFile(hFileFinder, &dta));
				FindClose(hFileFinder);
				//===This code promotes the "front" image to be the first result
				if (m_results.size() > 1)
				{
					INT foundFront = -1;
					for (size_t i = 0; i < m_results.size(); i++)
					{
						if (_tcsistr(m_results[i].c_str(), _T("front")) != NULL)
						{
							foundFront = i;
							break;
						}
					}
					if (foundFront > 0)
					{
						std::tstring tmp = m_results[0];
						m_results[0] = m_results[foundFront];
						m_results[foundFront] = tmp;
					}
				}
				//===End
			}
		}
	}
	m_curResult++;
	if (m_curResult >= 0 && m_curResult < (INT)m_results.size())
	{
		result.service = m_request.service;
		result.main = m_results[m_curResult].c_str();
		result.additionalInfo = _T("(local)"); 
		return TRUE;
	}
	return FALSE;
}

BOOL DirImageInfoProvider::CanHandle(ServiceEnum service) const
{
	switch (service)
	{
	case IInfoProvider::SRV_AlbumImage:
		return TRUE;
	}
	return FALSE;
}
LPCTSTR DirImageInfoProvider::GetModuleInfo(ModuleInfo mi) const
{
	switch (mi)
	{
	case IPI_UniqueID:		return _T("FOLD");
	case IPI_Name:			return _T("Folder Images");
	case IPI_Author:		return _T("Alex Economou");
	case IPI_VersionStr:	return _T("1");
	case IPI_VersionNum:	return _T("1");
	case IPI_Description:	return _T("Images from the tracks folder");
	case IPI_HomePage:		return _T("http://teenspirit.artificialspirit.com");
	case IPI_Email:			return _T("alex@artificialspirit.com");
	}
	return NULL;
}

//Strings WILL be Modified. (in order to avoid more memory allocations)
BOOL DirImageInfoProvider::IsIncluded(LPCTSTR strMain, LPCTSTR strFind, BOOL bIgnoreCase, LPCTSTR excludedCharacters)
{
	ASSERT(strMain != NULL && strFind != NULL);
	size_t strMainLen = _tcslen(strMain);
	if (strMainLen == 0)	return FALSE;
	size_t strFindLen = _tcslen(strFind);
	if (strFindLen == 0)	return FALSE;
	TCHAR sMain[MAX_PATH];
	TCHAR sFind[MAX_PATH];
	_tcscpy(sMain, strMain);
	_tcscpy(sFind, strFind);
	if (bIgnoreCase)
	{
		_tcslwr(sMain);
		_tcslwr(sFind);
	}
	if (excludedCharacters != NULL)
	{
		for (UINT i = 0; i < _tcslen(excludedCharacters); i++)
		{
			TCHAR strCh[2];
			strCh[1] = 0;
			strCh[0] = excludedCharacters[i];
			_tcsReplaceInl(sMain, strMainLen, strCh, _T(""));
			_tcsReplaceInl(sFind, strFindLen, strCh, _T(""));
		}
	}
	return _tcsstr(strMain, strFind) != NULL;
}

BOOL DirImageInfoProvider::HasMoreThanXFilesWithMinSizeY(LPCTSTR path, UINT maxNumFiles, UINT minFileSize)
{
	TCHAR dir[MAX_PATH];
	_tcsncpy(dir, path, MAX_PATH);
	TCHAR* pos = _tcsrchr(dir, '\\');
	if (pos != NULL)
	{
		pos++;
		*pos = 0;
		_tcscat(dir, _T("*.*"));
		WIN32_FIND_DATA fda;
		HANDLE f = FindFirstFile(dir, &fda);
		BOOL bRet = TRUE;
		UINT numFiles = 0;
		while (f != INVALID_HANDLE_VALUE && bRet)
		{
			if (fda.nFileSizeLow > minFileSize)
			{
				numFiles++;
				if (numFiles >= maxNumFiles)
				{
					FindClose(f);
					return TRUE;
				}
			}
			bRet = FindNextFile(f, &fda);
		}
		if (f != INVALID_HANDLE_VALUE)
			FindClose(f);
	}
	return FALSE;
	
}

BOOL DirImageInfoProvider::IsLegalPicture(LPCTSTR path, LPCTSTR albumName, BOOL bStrict)
{
	ASSERT(path != NULL);
	ASSERT(albumName != NULL);
#ifdef _DEBUG
	TRACE(_T("DirImageInfoProvider::IsLegalPicture. StrictSearch: %d\r\n"), bStrict);
	if (!bStrict && !IsIncluded(path, albumName, TRUE, _T("._ -'!?")))
		TRACE(_T("DirImageInfoProvider::IsLegalPicture. Strict wuld have failed (%s)!!!\r\n"), path);
#endif
	
	if (bStrict && !IsIncluded(path, albumName, TRUE, _T("._ -'!?")))
		return FALSE;
	if (_tcsstr(path, _T("AlbumArtSmall.jpg")) != NULL)
		return FALSE;
	if (_tcsstr(path, _T("}_Small.jpg")) != NULL)
		return FALSE;
	return TRUE;

}

LPCTSTR cDirImageSettings[] = 
{
	_T("Strict Max Files Count"),
	_T("Strict Max Files Size"),
	NULL
};

INT m_StrictCriteriaMaxFilesNum;
INT m_StrictCriteriaMaxFilesSize;


BOOL DirImageInfoProvider::GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const	
{
	setting = IConfigurable::SettingInfo();
	if (settingIndex < sizeof(cDirImageSettings) / sizeof(LPCTSTR) - 1)
		setting.name = cDirImageSettings[settingIndex];
	switch (settingIndex)
	{
	case 0:
	case 1:
		setting.type = IConfigurable::COVT_Int;
		return TRUE;
	}
	return FALSE;
}
INT DirImageInfoProvider::GetIntSetting(INT settingIndex) const				
{
	switch (settingIndex)
	{
	case 0:
		return m_StrictCriteriaMaxFilesNum;
	case 1:
		return m_StrictCriteriaMaxFilesSize;
	}
	ASSERT(0);
	return 0;
}

void DirImageInfoProvider::SetIntSetting(INT settingIndex, INT intVal)			
{
	switch (settingIndex)
	{
	case 0:
		if (intVal < 1)
			m_StrictCriteriaMaxFilesNum = 1;
		else if (intVal > 100)
			m_StrictCriteriaMaxFilesNum = 100;
		else
			m_StrictCriteriaMaxFilesNum = intVal;
		break;
	case 1:
		if (intVal < 50000)
			m_StrictCriteriaMaxFilesSize = 50000;
		else if (intVal > 10000000)
			m_StrictCriteriaMaxFilesSize = 10000000;
		else
			m_StrictCriteriaMaxFilesSize = intVal;
		break;
	default:
		ASSERT(0);
	}
}

