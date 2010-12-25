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
#include "CompositeInfoProvider.h"


#ifdef _UNITTESTING
#include "../CachedImageInfoProvider/CachedImageInfoProvider.h"
#include "../DirImageInfoProvider/DirImageInfoProvider.h"

BOOL TestCompositeInfoProvider()
{
	CompositeInfoProvider cip;
	CachedImageInfoProvider* pCIIP = new CachedImageInfoProvider;
	pCIIP->SetStoragePath(_T("d:\\TestStorage\\"));
	DirImageInfoProvider* pDIIP = new DirImageInfoProvider;
	cip.AddProvider(pCIIP, TRUE);
	cip.AddProvider(pDIIP, FALSE);


	delete pDIIP;

	return FALSE;//TODO
}
#endif

CompositeInfoProvider::CompositeInfoProvider()
{

}
CompositeInfoProvider::~CompositeInfoProvider()
{
	for (IPCollection::iterator it = m_ownedIPs.begin(); it != m_ownedIPs.end(); it++)
		delete *it;
}

IInfoProvider* CompositeInfoProvider::Clone() const
{
	//==='Respect ownership'
	CompositeInfoProvider* pIP = new CompositeInfoProvider;
	for (IPCollection::const_iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		BOOL bHasOwnership = FALSE;
		for (IPCollection::const_iterator itOwn = m_ownedIPs.begin(); itOwn != m_ownedIPs.end(); itOwn++)
		{
			if (*itOwn == *it)
			{
				bHasOwnership = TRUE;
				break;
			}
		}
		if (bHasOwnership)
			pIP->AddProvider((*it)->Clone(), TRUE);
		else
			pIP->AddProvider(*it, FALSE);
	}
	return pIP;
}

BOOL CompositeInfoProvider::OpenRequest(LPCTSTR requestString)
{
	//==='OpenRequest for EVERY provider'
	BOOL bSuccess = FALSE;
	for (IPCollection::iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		if ((*it)->OpenRequest(requestString))
			bSuccess = TRUE;
	}
	return bSuccess;
}
BOOL CompositeInfoProvider::NextResult()
{
	//==='If ANY NextResult succeeds then return true'
	for (IPCollection::iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		if ((*it)->NextResult())
			return TRUE;
	}
	return FALSE;
}

LPCTSTR CompositeInfoProvider::GetResult(ResultTypeEnum res) const
{
	//==='Return the FIRST that has a result'
	for (IPCollection::const_iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		if ((*it)->HasResult())
			return (*it)->GetResult(res);
	}
	return NULL;
}
BOOL CompositeInfoProvider::HasResult() const
{
	//==='If ANY HasResult succeeds then return true'
	for (IPCollection::const_iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		if ((*it)->HasResult())
			return TRUE;
	}
	return FALSE;
}

BOOL CompositeInfoProvider::AddResult(LPCTSTR info, ResultTypeEnum res)
{
	//==='Add to the FIRST that accepts it'
	for (IPCollection::iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		if ((*it)->AddResult(info, res))
			return TRUE;
	}
	return FALSE;

}
BOOL CompositeInfoProvider::ClearResult()
{
	//==='Clear the FIRST that has a result and can clear it'
	for (IPCollection::iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		if ((*it)->HasResult())
		{
			if ((*it)->ClearResult())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CompositeInfoProvider::CanHandle(LPCTSTR reqID) const
{
	//==='if ANY can handle it, it returns true'
	for (IPCollection::const_iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		if ((*it)->CanHandle(reqID))
			return TRUE;
	}
	return FALSE;

}

LPCTSTR CompositeInfoProvider::GetModuleInfo(ModuleInfo mi) const
{
	switch (mi)
	{
	case IPI_UniqueID:		return _T("COMP");
	case IPI_Name:			return _T("Composite");
	case IPI_Author:		return _T("Alex Economou");
	case IPI_VersionStr:	return _T("1.0");
	case IPI_VersionNum:	return _T("1");
	case IPI_Description:	return _T("Compines one or more IPs");
	}
	return NULL;
}

void CompositeInfoProvider::AddProvider(IInfoProvider* pIP, BOOL bTransferOwnerShip)
{
	m_IPs.push_back(pIP);
	if (bTransferOwnerShip)
		m_ownedIPs.push_back(pIP);
}

void CompositeInfoProvider::SetInternetHandle(LPVOID hInternet)
{
	//==='Config All'
	for (IPCollection::iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
		(*it)->SetInternetHandle(hInternet);
}

LPVOID CompositeInfoProvider::GetInternetHandle() const
{
	//==='Returns the FIRST not null'
	LPVOID ret = NULL;
	for (IPCollection::const_iterator it = m_IPs.begin(); it != m_IPs.end(); it++)
	{
		ret = (*it)->GetInternetHandle();
		if (ret != NULL)
			break;
	}
	return ret;
}

virtual BOOL GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const	{return FALSE;}
virtual INT GetIntSetting(INT settingIndex) const				{return 0;}
virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const		{return NULL;}
virtual void SetIntSetting(INT settingIndex, INT intVal)			{}
virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal)	{}



