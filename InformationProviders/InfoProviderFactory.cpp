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
#include "InfoProviderFactory.h"
#include "GoogleLyrics/GoogleLyricsProvider.h"
#include "LastFMProvider/LastFMProvider.h"
#include "AmazonProvider/AmazonProvider.h"
#include "ExternalInfoProvider/ExternalInfoProvider.h"
#include "AppSettings/AppSettings.h"



InfoProviderFactory::InfoProviderFactory():
m_bInitialized(FALSE),
m_hNet(0),
m_pAppSettings(NULL)
{

}

InfoProviderFactory::~InfoProviderFactory()
{
	//Clear All Information Providers
	IPInfoVector::iterator it = m_IPPrototypes.begin();
	while (it != m_IPPrototypes.end())
	{
		delete (*it).pIP;
		it++;
	}
}

IInfoProvider* InfoProviderFactory::CreateInfoProviderByUniqueID(LPCTSTR uniqueID)
{
	INT idx = 0;
	IInfoProvider* pIP = NULL;
	while ((pIP = GetItemByIndex(idx)) != NULL)
	{
		LPCTSTR oldID = pIP->GetModuleInfo(IInfoProvider::IPI_UniqueID);
		if (_tcscmp(uniqueID, oldID) == 0)
			return pIP->Clone();
		idx++;
	}
	return NULL;
}

IInfoProvider* InfoProviderFactory::CreateInfoProviderByService(IInfoProvider::ServiceEnum service)
{
	INT idx = 0;
	IInfoProvider* pIP = NULL;
	while ((pIP = GetItemByIndex(idx)) != NULL)
	{
		if (pIP->CanHandle(service))
			return CreateInfoProviderByUniqueID(pIP->GetModuleInfo(IInfoProvider::IPI_UniqueID));
		idx++;
	}
	return NULL;
}


INT InfoProviderFactory::LoadPlugins(LPCTSTR pluginsPath)
{
	TRACEST(_T("InfoProviderFactory::LoadPlugins"));
	ASSERT(pluginsPath != NULL);
	ASSERT(pluginsPath[_tcslen(pluginsPath) - 1] == '\\');
	INT count = 0;
	TCHAR spec[_MAX_PATH];
	_sntprintf(spec, _MAX_PATH, _T("%s*.dll"), pluginsPath);
	WIN32_FIND_DATA data;
	HANDLE h=FindFirstFile(spec,&data);
	if (h!=INVALID_HANDLE_VALUE) 
	{
		do 
		{
			TCHAR fname[_MAX_PATH];
			_sntprintf(fname, _MAX_PATH, _T("%s%s"), pluginsPath, data.cFileName);
			std::vector<ExternalInfoProvider*> ips = ExternalInfoProvider::Create(fname);
			for (size_t i = 0; i < ips.size(); i++)
			{
				IInfoProvider* pIP = ips[i];
				count++;
				pIP->SetInternetHandle(m_hNet);
				if (!RegisterInfoProvider(pIP, TRUE))
					delete pIP;
			}
		} 
		while (FindNextFile(h,&data));
		FindClose(h);
	}
	return count;
}



BOOL InfoProviderFactory::Init(HINTERNET hNet, AppSettings* pAppSettings)
{
	ASSERT(!m_bInitialized);
	m_hNet = hNet;
	m_pAppSettings = pAppSettings;
	return TRUE;
}

//INT InfoProviderFactory::GetInfoProviderTypesCount()
//{
//	return (INT) m_IPPrototypes.size();
//}


//IInfoProvider* InfoProviderFactory::GetItemByContainerIndex(INT idx)
//{
//	ASSERT(idx < GetInfoProviderTypesCount());
//	if (idx < GetInfoProviderTypesCount())
//		m_IPtypes[idx];
//	return NULL;
//}

//IInfoProvider* InfoProviderFactory::GetItemByUniqueID(LPCTSTR UniqueID)
//{
//	//IPMapper::iterator it = m_IPmapper.begin();
//	//while (it != m_IPmapper.end())
//	//{
//	//	if ((*it).second == TRUE)
//	//	{
//	//		IInfoProvider* pIP = (*it).first;
//	//		if (_tcscmp(UniqueID, pIP->GetModuleInfo(IInfoProvider::IPI_UniqueID)) == 0)
//	//		{
//	//			(*it).second = TRUE;
//	//			return pIP;
//	//		}	
//	//	}
//	//}
//	IInfoProvider* pIP = CreateInfoProvider(UniqueID);
//	if (pIP == NULL)
//	{
//		TRACE(_T("@1 InfoProviderLender::GetItemByUniqueID. Create Provider Failed\r\n"));
//		return NULL;
//	}
//	//m_IPmapper[pIP] = FALSE;
//	return pIP;
//}



BOOL InfoProviderFactory::RegisterInfoProvider(IInfoProvider* pIP, BOOL bPublicUse)
{
	//TRACEST(_T("InfoProviderFactory::RegisterInfoProvider"));
	ASSERT(pIP != NULL);
	if (pIP == NULL)
	{
		TRACE(_T("@1 InfoProviderFactory::RegisterInfoProvider pIP = NULL\r\n"));
		return FALSE;
	}
	IPInfoVector::iterator it = m_IPPrototypes.begin();
	LPCTSTR newUniqID = pIP->GetModuleInfo(IInfoProvider::IPI_UniqueID);
	while (it != m_IPPrototypes.end())
	{
		LPCTSTR uniqID = (*it).pIP->GetModuleInfo(IInfoProvider::IPI_UniqueID);
		if (_tcscmp(newUniqID, uniqID) == 0)
		{
			TRACE(_T("@2 InfoProviderFactory::RegisterInfoProvider. pIP Exists\r\n"));
			INT verExisting = 0, verNew = 0;
			LPCTSTR verExistingStr = (*it).pIP->GetModuleInfo(IInfoProvider::IPI_VersionNum);
			if (verExistingStr != NULL)
				verExisting = _ttoi(verExistingStr);
			LPCTSTR verNewStr = pIP->GetModuleInfo(IInfoProvider::IPI_VersionNum);
			if (verNewStr != NULL)
				verNew = _ttoi(verNewStr);
			if (verNew > verExisting)
			{
				delete (*it).pIP;
				(*it).pIP = pIP;
				(*it).bPublicUse = bPublicUse;
				return TRUE;
			}
			return FALSE;
		}
		it++;
	}
	if (m_pAppSettings != NULL)
	{
		LPCTSTR ipName = pIP->GetModuleInfo(IInfoProvider::IPI_Name);
		IConfigurable::SettingInfo setting;
		INT setIdx = 0;
		while (pIP->GetSettingInfo(setIdx, setting))
		{
			if (setting.type == IConfigurable::COVT_LPCTSTR)
			{
				std::tstring val;
				if (m_pAppSettings->Read(ipName, setting.name, val, _T("")))
					pIP->SetLPCTSTRSetting(setIdx, val.c_str());
			}
			else
			{
				INT val;
				if (m_pAppSettings->Read(ipName, setting.name, val, 0))
					pIP->SetIntSetting(setIdx, val);
			}
			setIdx++;
		}
	}
	m_IPPrototypes.push_back(IPInfo(pIP, bPublicUse));
	return TRUE;
}

IInfoProvider* InfoProviderFactory::GetItemByIndex(INT idx)
{
	ASSERT(idx >= 0);// && idx < (INT) m_IPPrototypes.size());

	if (!m_bInitialized)
	{
		IInfoProvider* pIP = NULL;
		pIP = new GoogleLyricsProvider;
		pIP->SetInternetHandle(m_hNet);
		if (!RegisterInfoProvider(pIP, TRUE))
			delete pIP;
		pIP = new LastFMProvider;
		pIP->SetInternetHandle(m_hNet);
		if (!RegisterInfoProvider(pIP, TRUE))
			delete pIP;
		pIP = new AmazonProvider;
		pIP->SetInternetHandle(m_hNet);
		if (!RegisterInfoProvider(pIP, TRUE))
			delete pIP;
		m_bInitialized = TRUE;
	}
	if (idx >= 0 && idx < (INT)m_IPPrototypes.size())
		return m_IPPrototypes[idx].pIP;
	return NULL;
}

BOOL InfoProviderFactory::ForPublicUse(INT idx) const
{
	ASSERT(idx >= 0);// && idx < (INT) m_IPPrototypes.size());
	if (idx >= 0 && idx < (INT)m_IPPrototypes.size())
		return m_IPPrototypes[idx].bPublicUse;
	return FALSE;
}




//void InfoProviderFactory::ReleaseItem(IInfoProvider* pIP)
//{
//	ASSERT(pIP != NULL);
//	if (m_IPmapper.find(pIP) == m_IPmapper.end())
//	{
//		TRACE(_T("@1 InfoProviderLender::ReleaseItem. Attempt to release an unkonwn item\r\n"));
//		ASSERT(0);//This is item is not included
//		return;
//	}
//	if (m_IPmapper[pIP] == TRUE)
//	{
//		TRACE(_T("@1 InfoProviderLender::ReleaseItem. Attempt to release an already free item\r\n"));
//		ASSERT(0);//This is item is not included
//		return;
//	}
//	m_IPmapper[pIP] = TRUE;
//}

void InfoProviderFactory::SaveSettings()
{
	TRACEST(_T("InfoProviderFactory::SaveSettings."));
	if (m_pAppSettings != NULL)
	{
		INT idx = 0;
		IInfoProvider* ip = GetItemByIndex(idx);
		while (ip != NULL)
		{
			LPCTSTR ipName = ip->GetModuleInfo(IInfoProvider::IPI_Name);
			IConfigurable::SettingInfo setting;
			INT setIdx = 0;
			while (ip->GetSettingInfo(setIdx, setting))
			{
				if (setting.type == IConfigurable::COVT_LPCTSTR)
					m_pAppSettings->Write(ipName, setting.name, ip->GetLPCTSTRSetting(setIdx));
				else
					m_pAppSettings->Write(ipName, setting.name, ip->GetIntSetting(setIdx));
				setIdx++;
			}
			idx++;
			ip = GetItemByIndex(idx);
		}

	}
}





