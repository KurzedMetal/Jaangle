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
#ifndef _CompositeInfoProvider_H_
#define _CompositeInfoProvider_H_

#include "../InfoProvider.h"

#ifdef _UNITTESTING
	BOOL TestCompositeInfoProvider();
#endif

class CompositeInfoProvider:public IInfoProvider
{
public:
	CompositeInfoProvider();
	virtual ~CompositeInfoProvider();

	virtual IInfoProvider* Clone() const;

	virtual BOOL OpenRequest(LPCTSTR requestString);
	virtual BOOL NextResult();

	virtual LPCTSTR GetResult(ResultTypeEnum res) const;
	virtual BOOL HasResult() const;

	virtual BOOL AddResult(LPCTSTR info, ResultTypeEnum res);
	virtual BOOL ClearResult();

	virtual BOOL CanHandle(LPCTSTR reqID) const;
	virtual LPCTSTR GetModuleInfo(ModuleInfo mi) const;

	virtual void SetInternetHandle(LPVOID hInternet);
	virtual LPVOID GetInternetHandle() const;

	//=== IConfigurable
	virtual BOOL GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const	{return FALSE;}
	virtual INT GetIntSetting(INT settingIndex) const				{return 0;}
	virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const		{return NULL;}
	virtual void SetIntSetting(INT settingIndex, INT intVal)			{}
	virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal)	{}

	void AddProvider(IInfoProvider* pIP, BOOL bTransferOwnerShip);


private:
	typedef std::vector<IInfoProvider*> IPCollection;
	IPCollection m_IPs;
	IPCollection m_ownedIPs;
	INT m_curResult;
	enum ServiceTypeEnum
	{
		SRV_None,
		SRV_ArtistPicture,
		SRV_AlbumPicture,
		SRV_Last
	};
	ServiceTypeEnum m_service;
};

#endif
