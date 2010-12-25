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
#ifndef _CachedImageInfoProvider_H_
#define _CachedImageInfoProvider_H_

#include "../InfoProvider.h"
#include <string>
#include <vector>

#ifdef _UNITTESTING
BOOL TestCachedImageInfoProvider();
#endif

class CachedImageInfoProvider:public IInfoProvider
{
#ifdef _UNITTESTING
public:
	static BOOL UnitTest();
#endif

public:
	CachedImageInfoProvider();
	virtual ~CachedImageInfoProvider();

	virtual IInfoProvider* Clone() const;

	virtual BOOL OpenRequest(const Request& request);
	virtual BOOL GetNextResult(Result& result);
	virtual const Request& GetRequest()							{return m_request;}

	virtual BOOL AddResult(const Result& result);
	virtual BOOL DeleteResult();

	virtual BOOL CanHandle(ServiceEnum service) const;
	virtual LPCTSTR GetModuleInfo(ModuleInfo mi) const;

	virtual void SetInternetHandle(LPVOID hInternet)	{}
	virtual LPVOID GetInternetHandle() const			{return NULL;}

	//=== IConfigurable
	virtual BOOL GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const	{return FALSE;}
	virtual INT GetIntSetting(INT settingIndex) const				{return 0;}
	virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const		{return NULL;}
	virtual void SetIntSetting(INT settingIndex, INT intVal)			{}
	virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal)	{}


	void SetStoragePath(LPCTSTR storagePath);


private:
	std::basic_string<TCHAR> m_storagePath;
	std::basic_string<TCHAR> m_Artist;
	std::basic_string<TCHAR> m_Album;
	//std::basic_string<TCHAR> m_result;
	std::vector<std::basic_string<TCHAR> > m_results;
	INT m_curResult;
	Request m_request;
private:
	BOOL GetFiles(LPCTSTR path, std::vector<std::basic_string<TCHAR> >& files);
	//BOOL GetFilePath(LPTSTR path);
	//BOOL DeleteFiles(LPCTSTR path);
	BOOL SaveFile(LPCTSTR mainPath, LPCTSTR extension, LPCTSTR srcPath);
	INT GetFileSize(LPCTSTR fileName);
	void GetArtistHash(LPTSTR hash, UINT hashLen);
	void GetAlbumHash(LPTSTR hash, UINT hashLen);

	void ConvertV2ArtistPictures();
	void ConvertV2AlbumPictures();

};

#endif
