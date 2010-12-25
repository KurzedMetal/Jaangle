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
#ifndef _ExternalInfoProvider_h_
#define _ExternalInfoProvider_h_

#include "../InfoProvider.h"
#include <vector>
#include <string>

class ExternalInfoProvider:public IInfoProvider
{
public:
	virtual ~ExternalInfoProvider();

	static std::vector<ExternalInfoProvider*> Create(LPCTSTR modulePath);

	virtual IInfoProvider* Clone() const;

	virtual BOOL OpenRequest(const Request& request);
	virtual BOOL GetNextResult(Result& result);
	virtual const Request& GetRequest();

	virtual BOOL AddResult(const Result& result);
	virtual BOOL DeleteResult();


	virtual BOOL CanHandle(ServiceEnum service) const;
	virtual LPCTSTR GetModuleInfo(ModuleInfo mi) const;

	virtual void SetInternetHandle(LPVOID hInternet);
	virtual LPVOID GetInternetHandle() const;

	//=== IConfigurable
	virtual BOOL GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const	{return FALSE;}
	virtual INT GetIntSetting(INT settingIndex) const				{return 0;}
	virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const		{return NULL;}
	virtual void SetIntSetting(INT settingIndex, INT intVal)			{}
	virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal)	{}

	BOOL IsValid()								{return m_pIP != NULL;}

private:
	ExternalInfoProvider(LPCTSTR dllPath, UINT ipIndex);
	std::basic_string<TCHAR> m_ModulePath;
	HMODULE m_module;
	IInfoProvider* m_pIP;
	UINT m_ipIndex;

};

#endif
