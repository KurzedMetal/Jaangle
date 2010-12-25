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
#ifndef _LastFMProvider_h_
#define _LastFMProvider_h_

#include "..\InfoProvider.h"
#include <string>
#include <vector>
#include <Wininet.h>

#ifdef _UNITTESTING
BOOL TestLastFMProvider();
#endif


class LastFMProvider:public IInfoProvider
{
#ifdef _UNITTESTING
public:
	static BOOL UnitTest();
#endif

public:
	LastFMProvider();
	virtual ~LastFMProvider();

	virtual IInfoProvider* Clone() const;

	virtual BOOL OpenRequest(const Request& request);
	virtual BOOL GetNextResult(Result& result);
	virtual const Request& GetRequest()							{return m_request;}


	virtual BOOL AddResult(const Result& result)				{return FALSE;}
	virtual BOOL DeleteResult()									{return FALSE;}

	virtual BOOL CanHandle(ServiceEnum service) const;
	virtual LPCTSTR GetModuleInfo(ModuleInfo mi) const;

	virtual void SetInternetHandle(LPVOID hInternet)			{m_hNet = hInternet;}
	virtual LPVOID GetInternetHandle() const					{return m_hNet;}


	//=== IConfigurable
	virtual BOOL GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const;
	virtual INT GetIntSetting(INT settingIndex) const;
	virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const		{return NULL;}
	virtual void SetIntSetting(INT settingIndex, INT intVal);
	virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal)	{}


private:
	std::basic_string<TCHAR> m_artist;
	std::basic_string<TCHAR> m_album;
	std::basic_string<TCHAR> m_result;

	std::vector<std::string> m_artistPictures;

	TCHAR m_TempFile[MAX_PATH];
	INT m_curResult;
	INT m_lastFMServer;

	Request m_request;
	HINTERNET m_hNet;

	//static void FixURLParameter(std::tstring& str);

	static BOOL IsWikiPageValid(LPCTSTR wikiPage);

private:
	BOOL GetArtistBio();
	BOOL GetArtistPicture();
	BOOL GetLastFMServer(std::basic_string<TCHAR>& server, INT lastFMServer);
	BOOL GetURL(LPCSTR start, LPSTR bf, UINT bfLen);
};



#endif
