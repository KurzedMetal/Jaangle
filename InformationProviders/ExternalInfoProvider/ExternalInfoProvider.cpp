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
#include "ExternalInfoProvider.h"

typedef UINT (*GetInfoProviderCount)();
typedef IInfoProvider *(*CreateInfoProvider)(UINT idx);
typedef BOOL (*DeleteInfoProvider)(IInfoProvider* pIP);


std::vector<ExternalInfoProvider*> ExternalInfoProvider::Create(LPCTSTR path)
{
	std::vector<ExternalInfoProvider*> ret;
	HMODULE mod = LoadLibrary(path);
	if (mod != NULL)
	{
		FARPROC gipcProc = GetProcAddress(mod, "GetInfoProviderCount");
		FARPROC cipProc = GetProcAddress(mod, "CreateInfoProvider");
		FARPROC dipProc = GetProcAddress(mod, "DeleteInfoProvider");
		if (gipcProc != NULL && cipProc != NULL && dipProc != NULL)
		{
			GetInfoProviderCount gipc = (GetInfoProviderCount) gipcProc;
			UINT ipCount = gipc();
			for (UINT i = 0; i < ipCount; i++)
			{
				ExternalInfoProvider* pExtIP = new ExternalInfoProvider(path, i);
				if (pExtIP->IsValid())
					ret.push_back(pExtIP);
				else
				{
					TRACE(_T("ExternalInfoProvider::Create. Invalid InfoProvider \r\n"));
					delete pExtIP;
				}
			}
		}
		FreeLibrary(mod);
	}
	return ret;
}


ExternalInfoProvider::ExternalInfoProvider(LPCTSTR dllPath, UINT ipIndex):
m_module(NULL),
m_pIP(NULL),
m_ipIndex(ipIndex)
{
	ASSERT(dllPath != NULL);
	m_module = LoadLibrary(dllPath);
	if (m_module != NULL)
	{
		m_ModulePath = dllPath;
		FARPROC cipProc = GetProcAddress(m_module, "CreateInfoProvider");
		if (cipProc)
		{
			CreateInfoProvider gipc = (CreateInfoProvider) cipProc;
			m_pIP = gipc(ipIndex);
		}
	}
}

ExternalInfoProvider::~ExternalInfoProvider()
{
	if (m_module != NULL)
	{
		if (m_pIP != NULL)
		{
			FARPROC proc = GetProcAddress(m_module, "DeleteInfoProvider");
			if (proc != NULL)
			{
				DeleteInfoProvider f = (DeleteInfoProvider) proc;
				f(m_pIP);
			}
		}
		FreeLibrary(m_module);
	}
}


IInfoProvider* ExternalInfoProvider::Clone() const
{
	IInfoProvider* pIP = new ExternalInfoProvider(m_ModulePath.c_str(), m_ipIndex);
	pIP->SetInternetHandle(pIP->GetInternetHandle());
	IConfigurableHelper::TransferConfiguration(*this, *pIP);
	return pIP;
}


BOOL ExternalInfoProvider::CanHandle(ServiceEnum service) const
{
	return m_pIP->CanHandle(service);
}

BOOL ExternalInfoProvider::OpenRequest(const Request& request)
{
	return m_pIP->OpenRequest(request);
}

BOOL ExternalInfoProvider::GetNextResult(Result& result)
{
	return m_pIP->GetNextResult(result);
}

const IInfoProvider::Request& ExternalInfoProvider::GetRequest()
{
	return m_pIP->GetRequest();
}


BOOL ExternalInfoProvider::AddResult(const Result& result)
{
	return m_pIP->AddResult(result);
}

BOOL ExternalInfoProvider::DeleteResult()
{
	return m_pIP->DeleteResult();
}

LPCTSTR ExternalInfoProvider::GetModuleInfo(ModuleInfo mi) const
{
	if (mi == IPI_ModuleFileName)
		return m_ModulePath.c_str();
	return m_pIP->GetModuleInfo(mi);
}
void ExternalInfoProvider::SetInternetHandle(LPVOID hInternet)
{
	m_pIP->SetInternetHandle(hInternet);
}
LPVOID ExternalInfoProvider::GetInternetHandle() const
{
	return m_pIP->GetInternetHandle();
}


