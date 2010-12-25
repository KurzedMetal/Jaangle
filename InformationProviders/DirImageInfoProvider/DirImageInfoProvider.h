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
#ifndef _DirImageInfoProvider_H_
#define _DirImageInfoProvider_H_

#include "../InfoProvider.h"
#include <string>
#include <vector>

class SQLManager;

#ifdef _UNITTESTING
BOOL TestDirImageInfoProvider();
#endif

class DirImageInfoProvider:public IInfoProvider
{
#ifdef _UNITTESTING
public:
	static BOOL UnitTest();
#endif

public:

	DirImageInfoProvider();
	virtual ~DirImageInfoProvider();

	virtual IInfoProvider* Clone() const;

	virtual BOOL OpenRequest(const Request& request);
	virtual BOOL GetNextResult(Result& result);
	virtual const Request& GetRequest()							{return m_request;}

	virtual BOOL AddResult(const Result& result)				{return FALSE;}
	virtual BOOL DeleteResult()									{return FALSE;}

	virtual BOOL CanHandle(ServiceEnum service) const;
	virtual LPCTSTR GetModuleInfo(ModuleInfo mi) const;

	virtual void SetInternetHandle(LPVOID hInternet)	{}
	virtual LPVOID GetInternetHandle() const			{return NULL;}

	//=== IConfigurable
	virtual BOOL GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const;
	virtual INT GetIntSetting(INT settingIndex) const;
	virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const		{return NULL;}
	virtual void SetIntSetting(INT settingIndex, INT intVal);
	virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal)	{}


	void SetSQLManager(SQLManager* pSM)		{m_pSQLManager = pSM;}

private:
	SQLManager* m_pSQLManager;
	INT m_curResult;
	Request m_request;
	std::basic_string<TCHAR> m_Artist;
	std::basic_string<TCHAR> m_Album;
	std::vector<std::basic_string<TCHAR> > m_results;

	BOOL IsLegalPicture(LPCTSTR path, LPCTSTR albumName, BOOL bStrict);

	static BOOL IsIncluded(LPCTSTR strMain, LPCTSTR strFind, BOOL bIgnoreCase, LPCTSTR excludedCharacters);
	static BOOL HasMoreThanXFilesWithMinSizeY(LPCTSTR path, UINT maxNumFiles, UINT minFileSize);
	BOOL GetFirstFileByArtistAlbum(LPTSTR path, LPCTSTR artist, LPCTSTR album);

	INT m_StrictCriteriaMaxFilesNum;
	INT m_StrictCriteriaMaxFilesSize;
};

#endif
