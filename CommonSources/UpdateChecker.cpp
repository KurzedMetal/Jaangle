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
#include "UpdateChecker.h"
#include "UrlDownloader.h"
#include "AsyncUrlDownloader.h"
#include "cStringUtils.h"

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

	LPBYTE lpVersionData = new BYTE[dwDataSize]; 
	if (!::GetFileVersionInfo(szModuleFileName, dwHandle, dwDataSize, (void**)lpVersionData) )
	{
		delete [] lpVersionData;
		return FALSE;
	}

	ASSERT(lpVersionData != NULL);

	UINT nQuerySize;
	VS_FIXEDFILEINFO* pVsffi;
	if ( ::VerQueryValue((void **)lpVersionData, _T("\\"), (void**)&pVsffi, &nQuerySize) )
	{
		fv.Major = HIWORD(pVsffi->dwFileVersionMS);
		fv.Minor = LOWORD(pVsffi->dwFileVersionMS);
		fv.Build = HIWORD(pVsffi->dwFileVersionLS);
		fv.SubBuild = LOWORD(pVsffi->dwFileVersionLS);
		delete [] lpVersionData;
		return TRUE;
	}

	delete [] lpVersionData;
	return FALSE;

}

UpdateChecker::UpdateChecker(void):
m_Status(S_Idle),
m_asyncHWND(0),
m_pFD(NULL)
{
}

UpdateChecker::~UpdateChecker(void)
{
	delete m_pFD;
	m_pFD = 0;
}

void UpdateChecker::Notify(IUrlDownloader* pCaller)
{
	ASSERT(m_pFD == pCaller);
	ASSERT(pCaller != NULL);
	if (pCaller->GetStatus() == IUrlDownloader::S_Success)
	{
		m_checkResult = CheckResult();
		BOOL bRet = Parse((LPCSTR)pCaller->GetBuffer(), pCaller->GetBufferSize());
		SetStatus(bRet ? S_Succeeded : S_Failed);
	}
	else
		SetStatus(S_Failed);
	//SetStatus(S_Succeeded);
	ASSERT(m_asyncHWND != NULL);
	if (m_asyncHWND)
		PostMessage(m_asyncHWND, MSG_UPDATECHECKER, 0, 0);

}

BOOL UpdateChecker::CheckForNewVersion(LPCTSTR URL2Check, FileVersion& curVersion, LPCTSTR progData/* = NULL*/, HWND asyncHWND/* = 0*/)
{
	ASSERT(URL2Check != NULL);
	if (GetStatus() == S_Working)
		return FALSE;
	if (curVersion == FileVersion(0,0,0,0))
		return FALSE;//Not the correct FileVersion
	if (m_pFD == NULL)
		m_pFD = new AsyncUrlDownloader;
	if (m_pFD->GetStatus() == IUrlDownloader::S_Working)
		return FALSE;
	SetStatus(S_Working);
	BOOL bRet = TRUE;
	std::tstring url(URL2Check);
	url += _T("?curversion=");
	TCHAR bf[30];
	curVersion.toString(bf);
	url += bf;
	if (progData != NULL && progData[0] != 0)
	{
		url += _T("&progData=");
		url += progData;
	}
	if (asyncHWND > 0)
	{
		m_asyncHWND = asyncHWND;
		m_pFD->SetAsyncTarget(this);
		m_pFD->DownloadURL(url.c_str());
#ifdef _DEBUG
		//ShellExecute(0,_T("open"), url.c_str(), 0,0,0);
#endif
	}
	else
	{
		UrlDownloader dl;
		bRet = dl.DownloadURL(URL2Check);
		if (bRet)
			bRet = Parse((LPCSTR)dl.GetBuffer(), dl.GetBufferSize());
		SetStatus(bRet ? S_Succeeded : S_Failed);
		return TRUE;
	}
	return bRet;
}

const UpdateChecker::CheckResult* UpdateChecker::GetCheckResult() const
{
	if (GetStatus() == S_Succeeded)
		return &m_checkResult;
	return NULL;
}

//#include "XmlNodeWrapper.h"

std::tstring UpdateChecker::XMLGetText(LPCTSTR xml, LPCTSTR tag)
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

BOOL UpdateChecker::Parse(LPCSTR html, UINT size)
{
#ifdef _UNICODE
	TCHAR* res = HTMLText2Unicode(html, size);
#else
	TCHAR* res = new TCHAR[size + 1];
	_tcsncpy(res, &file[0], file.size());
	res[html] = 0;
#endif

	std::tstring xmlTag = XMLGetText(res, _T("version"));
	if (xmlTag.empty() || xmlTag.size() >= 100)
		return FALSE;
	m_checkResult.onlineVersion = FileVersion();
	TCHAR bf[100];
	_tcsncpy(bf, xmlTag.c_str(), xmlTag.size());
	bf[xmlTag.size()] = 0;
	TCHAR* token = 0;
	token = _tcstok(bf, _T("."));
	INT curToken = 0;
	while (token != NULL)
	{
		INT num = _ttoi(token);
		switch (curToken)
		{
		case 0:
			m_checkResult.onlineVersion.Major = num;
			break;
		case 1:
			m_checkResult.onlineVersion.Minor = num;
			break;
		case 2:
			m_checkResult.onlineVersion.Build = num;
			break;
		case 3:
			m_checkResult.onlineVersion.SubBuild = num;
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

	m_checkResult.downloadURL = XMLGetText(res, _T("downloadurl"));
	m_checkResult.infoURL = XMLGetText(res, _T("infourl"));
	xmlTag = XMLGetText(res, _T("severity"));
	if (!xmlTag.empty())
	{
		INT num = _ttoi(xmlTag.c_str());
		if (num >= SEV_NoNeed && num< SEV_Last)
			m_checkResult.severity = (UpdateSeverity)num;
	}
	delete res;


	//m_checkResult.severity = SEV_Maintenance;
	//m_checkResult.currentVersion = FileVersion(1,1);
	//m_checkResult.onlineVersion = FileVersion(1,2);
	//m_checkResult.downloadURL = _T("http://www.defsite.com/test.zip");
	//m_checkResult.infoURL = _T("http://www.defsite.com/test.php?maj=1.2.32.0");
	return TRUE;

}

UpdateChecker::Status UpdateChecker::GetStatus() const
{
	return m_Status;
}

void UpdateChecker::SetStatus(Status st)
{
	m_Status = st;
}




