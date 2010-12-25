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
#include "StdAfx.h"
#include "NewVersionChecker.h"
#include "cStringUtils.h"
#include "WebPageUtilities.h"

#pragma comment(lib, "version.lib")

BOOL GetFileVersion(FileVersion& fv, HMODULE module/* = 0*/)
{
	TCHAR szModuleFileName[MAX_PATH];
	if (module == 0)
		module = AfxGetInstanceHandle();
	if (GetModuleFileName(module, szModuleFileName, sizeof(szModuleFileName)) == 0) 
		return FALSE;
	DWORD dwHandle;     
	DWORD dwDataSize = ::GetFileVersionInfoSize(szModuleFileName, &dwHandle); 
	if ( dwDataSize == 0 ) 
		return FALSE;

	void* lpVersionData = malloc(dwDataSize);//new BYTE[dwDataSize]; 
	if (!::GetFileVersionInfo(szModuleFileName, dwHandle, dwDataSize, lpVersionData) )
	{
		delete [] lpVersionData;
		return FALSE;
	}

	ASSERT(lpVersionData != NULL);

	UINT nQuerySize;
	VS_FIXEDFILEINFO* pVsffi;
	if ( ::VerQueryValue(lpVersionData, _T("\\"), (void**)&pVsffi, &nQuerySize) )
	{
		fv.Major = HIWORD(pVsffi->dwFileVersionMS);
		fv.Minor = LOWORD(pVsffi->dwFileVersionMS);
		fv.Build = HIWORD(pVsffi->dwFileVersionLS);
		fv.SubBuild = LOWORD(pVsffi->dwFileVersionLS);
		free(lpVersionData);
		return TRUE;
	}
	free(lpVersionData);
	return FALSE;
}

NewVersionChecker::CheckResult NewVersionChecker::CheckForNewVersion(
	HINTERNET hNet, LPCTSTR URL2Check, LPCTSTR progData)
{
	CheckResult checkRes;
	ASSERT(URL2Check != NULL);
	BOOL bRet = TRUE;
	FileVersion curVersion;
	GetFileVersion(curVersion, NULL);
	TCHAR sVer[30];
	curVersion.toString(sVer, 30);

	TCHAR url[1000];
	_sntprintf(url, 1000, _T("%s?curversion=%s&progdata=%s"), 
		URL2Check,
		sVer,
		progData);
	std::wstring page;
	if (DownloadWebPageUnicode(page, hNet, url))
	{
		std::tstring xmlTag = XMLGetText(page.c_str(), _T("version"));
		if (!xmlTag.empty() && xmlTag.size() < 100)
		{
			checkRes.error = ERR_OK;
			String2FileVersion(checkRes.stableVersion, xmlTag.c_str());
			xmlTag = XMLGetText(page.c_str(), _T("betaversion"));
			String2FileVersion(checkRes.betaVersion, xmlTag.c_str());
			checkRes.downloadURL = XMLGetText(page.c_str(), _T("downloadurl"));
			checkRes.infoURL = XMLGetText(page.c_str(), _T("infourl"));
		}
	}
	return checkRes;
}

void NewVersionChecker::String2FileVersion(FileVersion& fv, LPCTSTR str)
{
	TCHAR bf[100];
	_tcsncpy(bf, str, 100);
	bf[99] = 0;
	TCHAR* token = 0;
	token = _tcstok(bf, _T("."));
	INT curToken = 0;
	while (token != NULL)
	{
		INT num = _ttoi(token);
		switch (curToken)
		{
		case 0:
			fv.Major = num;
			break;
		case 1:
			fv.Minor = num;
			break;
		case 2:
			fv.Build = num;
			break;
		case 3:
			fv.SubBuild = num;
			break;
		default:
			num = -1;
			ASSERT(0);
			break;
		}
		if (num == -1)
			break;
		curToken++;
		token = _tcstok(NULL, _T("."));
	}
}

std::tstring NewVersionChecker::XMLGetText(LPCTSTR xml, LPCTSTR tag)
{
	ASSERT(xml != NULL && tag != NULL);
	std::tstring properTag = _T("<");
	properTag += tag;
	const TCHAR* startPos = _tcsistr(xml, properTag.c_str());
	if (startPos != NULL)
		startPos = _tcschr(startPos, '>');
	if (startPos != NULL)
	{
		startPos++;
		properTag = _T("</");
		properTag += tag;
		const TCHAR* endPos = _tcsistr(startPos, properTag.c_str());
		if (endPos != NULL)
		{
			if (endPos - startPos > 0)
			{
				properTag.clear();
				properTag.append(startPos, endPos - startPos);
				return properTag;
			}
		}
	}
	return std::tstring();
}



