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
#include "CachedImageInfoProvider.h"
#include "cStringUtils.h"
#include "shlwapi.h"
#include "cMD5.h"

#ifdef _UNITTESTING
LPCTSTR tempDir = _T("D:\\temp");
void DeleteTempDir()
{
	SHFILEOPSTRUCT fop;
	fop.hwnd = 0;
	fop.wFunc = FO_DELETE;
	fop.pFrom = tempDir;
	fop.pTo = NULL;
	fop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	fop.fAnyOperationsAborted = FALSE;
	fop.hNameMappings = NULL;
	fop.lpszProgressTitle = _T("Deleting...");
	SHFileOperation(&fop);

};
const int lastSizeOf = 140;

BOOL CachedImageInfoProvider::UnitTest()
{
	if (lastSizeOf != sizeof(CachedImageInfoProvider))
		TRACE(_T("TestCachedImageInfoProvider. Object Size Changed. Was: %d - Is: %d\r\n"), lastSizeOf, sizeof(CachedImageInfoProvider));
	DeleteTempDir();
	CachedImageInfoProvider ip;
	ip.SetStoragePath(tempDir);
	Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = _T("Nick Cave");
	if (ip.OpenRequest(req))
	{
		Result res;
		UNITTEST(ip.GetNextResult(res) == FALSE);
		res.main = _T("D:\\My Photos\\2007\\03 Canada-Cuba\\IMG_0594.jpg");
		res.additionalInfo = _T("");
		res.service = IInfoProvider::SRV_ArtistImage;
		ip.AddResult(res);
		res.main = _T("D:\\My Photos\\2007\\03 Canada-Cuba\\IMG_0595.jpg");
		ip.AddResult(res);
	}
	if (ip.OpenRequest(req))
	{
		DWORD resNum = 0;
		Result res;
		while (ip.GetNextResult(res))
		{
			UNITTEST(res.IsValid());
			TRACE(_T("Result %d. %s\r\n"), resNum++, res.main);
		}
	}
	req.service = IInfoProvider::SRV_AlbumImage;
	req.artist = _T("Nick Cave");
	req.album = _T("Kapoio ALbum");
	if (ip.OpenRequest(req))
	{
		Result res;
		res.additionalInfo = _T("");
		res.service = IInfoProvider::SRV_ArtistImage;
		UNITTEST(ip.GetNextResult(res) == FALSE);
		res.main = _T("D:\\My Photos\\2007\\03 Canada-Cuba\\IMG_0594.jpg");
		ip.AddResult(res);
		res.main = _T("D:\\My Photos\\2007\\03 Canada-Cuba\\IMG_0595.jpg");
		ip.AddResult(res);
	}
	if (ip.OpenRequest(req))
	{
		DWORD resNum = 0;
		Result res;
		while (ip.GetNextResult(res))
		{
			UNITTEST(res.IsValid());
			TRACE(_T("Result %d. %s\r\n"), resNum++, res.main);
		}
	}
	DeleteTempDir();
	return TRUE;
}

#endif


CachedImageInfoProvider::CachedImageInfoProvider():
m_request(SRV_First),
m_curResult(-1)
{

}
CachedImageInfoProvider::~CachedImageInfoProvider()
{
}

IInfoProvider* CachedImageInfoProvider::Clone() const
{
	CachedImageInfoProvider* pIP = new CachedImageInfoProvider;
	pIP->SetInternetHandle(GetInternetHandle());
	IConfigurableHelper::TransferConfiguration(*this, *pIP);
	pIP->SetStoragePath(m_storagePath.c_str());
	return pIP;
}


BOOL CachedImageInfoProvider::CanHandle(ServiceEnum service) const
{
	switch (service)
	{
	case IInfoProvider::SRV_ArtistImage:
	case IInfoProvider::SRV_AlbumImage:
		return TRUE;
	}
	return FALSE;
}

BOOL CachedImageInfoProvider::OpenRequest(const Request& request)
{
	m_curResult = -1;
	ASSERT(request.IsValid());
	if (!request.IsValid())
		return FALSE;
	m_results.clear();
	switch (request.service)
	{
	case IInfoProvider::SRV_ArtistImage:
		m_Artist = request.artist;
		m_request.artist = m_Artist.c_str();
		m_request.album = NULL;
		break;
	case IInfoProvider::SRV_AlbumImage:
		m_Artist = request.artist;
		m_Album = request.album;
		m_request.artist = m_Artist.c_str();
		m_request.album = m_Album.c_str();
		break;
	default:
		return FALSE;
	}
	m_request.service = request.service;
	return TRUE;
}

BOOL CachedImageInfoProvider::GetFiles(LPCTSTR path, std::vector<std::basic_string<TCHAR> >& files)
{
	WIN32_FIND_DATA dta;
	HANDLE hFileFinder = FindFirstFile(path, &dta);
	if (hFileFinder != INVALID_HANDLE_VALUE)
	{
		do 
		{
			TCHAR bf[MAX_PATH];
			bf[MAX_PATH - 1] = 0;
			_sntprintf(bf, MAX_PATH, _T("%s%s"), m_storagePath.c_str(), dta.cFileName);
			if (bf[MAX_PATH - 1] == 0)
				files.push_back(bf);
		} while(FindNextFile(hFileFinder, &dta));
		FindClose(hFileFinder);
	}
	return files.size() > 0;
}

void CachedImageInfoProvider::GetArtistHash(LPTSTR hash, UINT hashLen)
{
	cMD5 md5;
	TCHAR hashString[500];
	INT posStart = 0;
	if (_tcsnicmp(m_Artist.c_str(), _T("the "), 4) == 0)
		posStart = 4;
	_sntprintf(hashString, 500,_T("%s"), &m_Artist.c_str()[posStart]);
	_tcslwr(hashString);
	_tcsncpy(hash, CA2CT(md5.CalcMD5FromWString(hashString)), hashLen);
}

void CachedImageInfoProvider::GetAlbumHash(LPTSTR hash, UINT hashLen)
{
	cMD5 md5;
	TCHAR hashString[500];
	INT posStart = 0;
	if (_tcsnicmp(m_Artist.c_str(), _T("the "), 4) == 0)
		posStart = 4;
	_sntprintf(hashString, 500,_T("%s-%s"), &m_Artist.c_str()[posStart], m_Album.c_str());
	_tcslwr(hashString);
	_tcsncpy(hash, CA2CT(md5.CalcMD5FromWString(hashString)), hashLen);
}


void CachedImageInfoProvider::ConvertV2ArtistPictures()
{
	cMD5 md5;
	TCHAR path[MAX_PATH];
	//--- Format v2
	_sntprintf(path, MAX_PATH,_T("%s%s??.???"), m_storagePath.c_str(), CA2CT(md5.CalcMD5FromString(CT2CA(m_Artist.c_str()))));
	WIN32_FIND_DATA dta;
	HANDLE hFileFinder = FindFirstFile(path, &dta);
	if (hFileFinder != INVALID_HANDLE_VALUE)
	{
		do 
		{
			TCHAR bf[MAX_PATH];
			bf[MAX_PATH - 1] = 0;
			_sntprintf(bf, MAX_PATH, _T("%s%s"), m_storagePath.c_str(), dta.cFileName);
			if (bf[MAX_PATH - 1] == 0)
			{
				Result res;
				res.service = SRV_ArtistImage;
				res.main = bf;
				AddResult(res);
				DeleteFile(bf);
			}
		} while(FindNextFile(hFileFinder, &dta));
		FindClose(hFileFinder);
	}

	//--- Format v1
	//TCHAR artist[MAX_PATH];
	//_tcsncpy(artist, m_Artist.c_str(), MAX_PATH);
	//_tcschrrep(INVALID_CHARS_FOR_FILENAME, '_', artist);
	//_sntprintf(path, MAX_PATH,_T("%s%.20s??.???"), m_storagePath.c_str(), artist);
	//GetFiles(path, m_results);


}

void CachedImageInfoProvider::ConvertV2AlbumPictures()
{
	cMD5 md5;
	TCHAR path[MAX_PATH];
	//--- Format v2
	_sntprintf(path, MAX_PATH,_T("%s-%s"), m_Artist.c_str(), m_Album.c_str());
	_sntprintf(path, MAX_PATH,_T("%s%s??.???"), m_storagePath.c_str(), CA2CT(md5.CalcMD5FromString(CT2CA(path))));
	WIN32_FIND_DATA dta;
	HANDLE hFileFinder = FindFirstFile(path, &dta);
	if (hFileFinder != INVALID_HANDLE_VALUE)
	{
		do 
		{
			TCHAR bf[MAX_PATH];
			bf[MAX_PATH - 1] = 0;
			_sntprintf(bf, MAX_PATH, _T("%s%s"), m_storagePath.c_str(), dta.cFileName);
			if (bf[MAX_PATH - 1] == 0)
			{
				Result res;
				res.service = SRV_AlbumImage;
				res.main = bf;
				AddResult(res);
				DeleteFile(bf);
			}
		} while(FindNextFile(hFileFinder, &dta));
		FindClose(hFileFinder);
	}

	//--- Format v1
	//TCHAR artist[MAX_PATH];
	//_tcsncpy(artist, m_Artist.c_str(), MAX_PATH);
	//_tcschrrep(INVALID_CHARS_FOR_FILENAME, '_', artist);
	//_sntprintf(path, MAX_PATH,_T("%s%.20s??.???"), m_storagePath.c_str(), artist);
	//GetFiles(path, m_results);


}



BOOL CachedImageInfoProvider::GetNextResult(Result& result)
{
	if (m_curResult == -1)
	{
		m_results.clear();
		TCHAR path[MAX_PATH];
		TCHAR hash[500];
		switch (m_request.service)
		{
		case SRV_ArtistImage:
			ConvertV2ArtistPictures();
			GetArtistHash(hash, 500);
			_sntprintf(path, MAX_PATH, _T("%sAR_%s??.*"), m_storagePath.c_str(), hash);
			GetFiles(path, m_results);
			break;
		case SRV_AlbumImage:
			ConvertV2AlbumPictures();
			GetAlbumHash(hash, 500);
			_sntprintf(path, MAX_PATH,_T("%sAL_%s??.*"), m_storagePath.c_str(), hash);
			GetFiles(path, m_results);
			break;
		}
	}
	m_curResult++;
	if (m_curResult < 0 || m_curResult >= (INT)m_results.size())	
		return FALSE;
	result.main = m_results[m_curResult].c_str();
	result.additionalInfo = m_storagePath.c_str();
	return TRUE;
}

void CachedImageInfoProvider::SetStoragePath(LPCTSTR storagePath)		
{
	ASSERT(storagePath != NULL);
	m_storagePath = storagePath;
	if (m_storagePath[m_storagePath.size() - 1] != '\\')
		m_storagePath += '\\';
	if (!PathFileExists(m_storagePath.c_str()))
		CreateDirectory(m_storagePath.c_str(), NULL);
}


BOOL CachedImageInfoProvider::AddResult(const Result& result)	
{
	ASSERT(result.IsValid());
	TCHAR path[MAX_PATH];
	TCHAR hash[500];
	switch (result.service)
	{
	case SRV_ArtistImage:
		{
			GetArtistHash(hash, 500);
			_sntprintf(path, MAX_PATH,_T("%sAR_%s"), m_storagePath.c_str(), hash);
			return SaveFile(path, _T("jpg"), result.main);
		}
		break;
	case SRV_AlbumImage:
		{
			GetAlbumHash(hash, 500);
			_sntprintf(path, MAX_PATH,_T("%sAL_%s"), m_storagePath.c_str(), hash);
			return SaveFile(path, _T("jpg"), result.main);
		}
		break;
	}

	return FALSE;
}

INT CachedImageInfoProvider::GetFileSize(LPCTSTR fileName)
{
	ASSERT(fileName != NULL);
	HANDLE f= CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		DWORD fs = ::GetFileSize(f, NULL);
		CloseHandle(f);
		return INT(fs);
	}
	return -1;
}

BOOL CachedImageInfoProvider::SaveFile(LPCTSTR mainPath, LPCTSTR extension, LPCTSTR srcPath)
{
	TCHAR trgPathTemplate[MAX_PATH];
	_sntprintf(trgPathTemplate, MAX_PATH, _T("%s??.%s"), mainPath, extension);
	INT srcFileSize = GetFileSize(srcPath);
	WIN32_FIND_DATA dta;
	HANDLE hFileFinder = FindFirstFile(trgPathTemplate, &dta);
	if (hFileFinder != INVALID_HANDLE_VALUE)
	{
		BOOL bFoundSameSize = FALSE;
		do
		{
			if (dta.nFileSizeLow == srcFileSize)
			{
				bFoundSameSize = TRUE;
				break;
			}

		}while (FindNextFile(hFileFinder, &dta));
		FindClose(hFileFinder);
		if (bFoundSameSize)
			return FALSE;
	}
	TCHAR newPath[MAX_PATH];
	for (int i=0; i<0xff; i++)
	{
		_sntprintf(newPath, MAX_PATH, _T("%s%0.2X.%s"), mainPath, i, extension);
		if (!PathFileExists(newPath))//Does not exist
			return CopyFile(srcPath, newPath, TRUE) != 0;
	}
	return FALSE;
}

//BOOL CachedImageInfoProvider::DeleteFiles(LPCTSTR path)
//{
//	ASSERT(path != NULL);
//	WIN32_FIND_DATA dta;
//	HANDLE hFileFinder = FindFirstFile(path, &dta);
//	if (hFileFinder == INVALID_HANDLE_VALUE)
//		return TRUE;//Files Not Found
//	BOOL bRet = TRUE;
//	TCHAR bf[MAX_PATH];
//	while (TRUE)
//	{
//		bf[MAX_PATH - 1] = 0;
//		_sntprintf(bf, MAX_PATH, _T("%s%s"), m_storagePath.c_str(), dta.cFileName);
//		if (bf[MAX_PATH - 1] != 0)
//		{
//			ASSERT(!_T("File Name TOO Long"));
//			bRet = FALSE;
//			break;
//		}
//		if (!DeleteFile(bf))
//		{
//			bRet = FALSE;
//			break;
//		}
//		if (!FindNextFile(hFileFinder, &dta))
//			break;
//	}
//	FindClose(hFileFinder);
//	return bRet;
//}
BOOL CachedImageInfoProvider::DeleteResult()
{
	if (m_curResult >= 0 && m_curResult < (INT)m_results.size())
		return DeleteFile(m_results[m_curResult].c_str());
	return FALSE;
}

LPCTSTR CachedImageInfoProvider::GetModuleInfo(ModuleInfo mi) const
{
	switch (mi)
	{
	case IPI_UniqueID:		return _T("CACH");
	case IPI_Name:			return _T("Cached Images");
	case IPI_Author:		return _T("Alex Economou");
	case IPI_VersionStr:	return _T("1");
	case IPI_VersionNum:	return _T("1");
	case IPI_Description:	return _T("Cached Images");
	case IPI_HomePage:		return _T("http://teenspirit.artificialspirit.com");
	case IPI_Email:			return _T("alex@artificialspirit.com");
	}
	return NULL;
}
