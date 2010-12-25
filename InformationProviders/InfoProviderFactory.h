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
#ifndef _InfoProviderFactory_H_
#define _InfoProviderFactory_H_

#include <vector>
#include <map>
#include <WinInet.h>
#include "InfoProvider.h"
class AppSettings;

//class InfoProviderFactory;

//InfoProviderFactory is the OWNER and CREATOR of all the InformationProviders
//	- Creates all the various InfoProviders
//	- Owns all the created InfoProviders
//	- "Lends" objects using the "GetItem..." Function. The client MUST release the 
//		InfoProvider when it does not need it anymore
//	- It is thread-safe
//	- Has a list of all available Info Provider
//	- Keeps track of all the created InfoProvider Objects (DEBUG)

class InfoProviderFactory
{
public:
	InfoProviderFactory();
	~InfoProviderFactory();
	
	BOOL Init(HINTERNET hNet, AppSettings* pAppSettings);
	INT LoadPlugins(LPCTSTR pluginsPath);
	INT UnLoadPlugins();

	//INT GetInfoProviderTypesCount();

	//LPCTSTR GetModuleInfo(INT idx, IInfoProvider::ModuleInfo mi) const;
	//BOOL CanHandle(INT idx, LPCTSTR reqString) const;
	IInfoProvider* GetItemByIndex(INT idx);
	//IInfoProvider* GetItemByUniqueID(LPCTSTR providerName);
	IInfoProvider* CreateInfoProviderByUniqueID(LPCTSTR uniqueID);
	IInfoProvider* CreateInfoProviderByService(IInfoProvider::ServiceEnum service);

	BOOL RegisterInfoProvider(IInfoProvider* pIP, BOOL bPublicUse);
	BOOL ForPublicUse(INT idx) const;

	void SaveSettings();

private:
	struct IPInfo
	{
		IPInfo(IInfoProvider* pip, BOOL publicuse):
	pIP(pip),bPublicUse(publicuse){}
		IInfoProvider* pIP;
		BOOL bPublicUse;
	};
	typedef std::vector<IPInfo> IPInfoVector;
	IPInfoVector m_IPPrototypes;
	BOOL m_bInitialized;
	HINTERNET m_hNet;
	AppSettings* m_pAppSettings;
private:

};

#endif
