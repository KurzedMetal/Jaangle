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
#include "ServicesRegistrationSrv.h"
#include "stlStringUtils.h"
#include "WebPageUtilities.h"
#include "IconSerialization.h"
#include <xmllite.h>
#pragma comment( lib, "xmllite.lib" )
#include "XmlLiteUtilities.h"


#define PLACEHOLDER_ARTIST	_T("%artist%")
#define PLACEHOLDER_ALBUM	_T("%album%")
#define PLACEHOLDER_TRACKNO _T("%trackno%")
#define PLACEHOLDER_TITLE	_T("%title%")

BOOL ServiceInfo::CanHandle(ServiceTypeEnum type)
{
	if (AppliesTo == NULL)
		return FALSE;
	LPCTSTR f = NULL;
	switch (type)
	{
	case STYPE_ArtistInfo:
		f = PLACEHOLDER_ARTIST;
		break;
	case STYPE_AlbumInfo:
		f = PLACEHOLDER_ALBUM;
		break;
	case STYPE_TrackInfo:
		f = PLACEHOLDER_TITLE;
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	return _tcsstr(AppliesTo, f) != NULL;
	//return m_services[ID].AppliesTo.find(f, 0) != std::string::npos;
}

BOOL ServicesRegistrationSrv::AddService(ServiceInfo si)
{
	ASSERT(si.Name != NULL && si.Action != NULL);
	ServiceInfoStorage srv;
	srv.Name = si.Name;
	srv.Action = si.Action;
	if (si.AppliesTo != NULL)
		srv.AppliesTo = si.AppliesTo;
	if (si.Creator != NULL)
		srv.Creator = si.Creator;
	srv.hIcon = si.hIcon;
	m_services.push_back(srv);
	return TRUE;
}

UINT ServicesRegistrationSrv::GetServicesCount()
{
	return (UINT) m_services.size();
}



BOOL ServicesRegistrationSrv::GetServiceInfo(UINT ID, ServiceInfo& serviceInfo)
{
	ASSERT(ID < GetServicesCount());
	if (ID < GetServicesCount())
	{
		serviceInfo.Action = m_services[ID].Action.c_str();
		serviceInfo.Creator = m_services[ID].Creator.c_str();
		serviceInfo.AppliesTo = m_services[ID].AppliesTo.c_str();
		serviceInfo.Name = m_services[ID].Name.c_str();
		serviceInfo.hIcon = m_services[ID].hIcon;
		return TRUE;
	}
	return FALSE;
}
BOOL ServicesRegistrationSrv::Execute(ServiceInfo& service, ServicesExecuteInfo& inf, IWebViewer* pWebViewer)
{
	TRACEST(_T("ServicesRegistrationSrv::Execute"));
	LPCTSTR const emptyString = _T("");
	std::tstring cmd(service.Action);
	std::tstring fixedString;
	if (inf.artist != NULL)
	{
		fixedString.clear();
		if (!URLEncode(fixedString, inf.artist))
			fixedString.clear();
		replace(cmd, std::tstring(PLACEHOLDER_ARTIST), fixedString, 0);
	}
	else
		replace(cmd, PLACEHOLDER_ARTIST, _T(""), 0);

	if (inf.album != NULL)
	{
		fixedString.clear();
		if (!URLEncode(fixedString, inf.album))
			fixedString.clear();
		replace(cmd, std::tstring(PLACEHOLDER_ALBUM), fixedString, 0);
	}
	else
		replace(cmd, PLACEHOLDER_ALBUM, _T(""), 0);

	if (inf.title != NULL)
	{
		fixedString.clear();
		if (!URLEncode(fixedString, inf.title))
			fixedString.clear();
		replace(cmd, std::tstring(PLACEHOLDER_TITLE), fixedString, 0);
	}
	else
		replace(cmd, PLACEHOLDER_TITLE, _T(""), 0);


	if (_tcsnicmp(cmd.c_str(), _T("web"), 3) == 0)
	{
		replace(cmd, _T("\""), _T("%22"));
		LPCTSTR realCmd = &cmd.c_str()[4];
		BOOL bExecuted = FALSE;
		if (pWebViewer != NULL)
			bExecuted = pWebViewer->OpenURL(realCmd);
		if (bExecuted == FALSE)
			bExecuted = ShellExecute(0, _T("open"), realCmd, NULL, NULL, NULL) != 0;
		return bExecuted;
	}
	return FALSE;
}


//#include "XmlNodeWrapper.h"
//BOOL ServicesRegistrationSrv::ImportXMLFile(LPCTSTR FilePath)
//{
//	CXmlDocumentWrapper xml;
//	if (!xml.Load(FilePath))
//		return FALSE;
//	CXmlNodeWrapper root(xml.AsNode());
//	CString name, action, appliesto, creator;
//	for (int i = 0; i < root.NumNodes(); i++)
//	{
//		name.Empty();
//		action.Empty();
//		appliesto.Empty();
//		creator.Empty();
//		CXmlNodeWrapper services(root.GetNode(i));
//		for (int j = 0; j < services.NumAttributes(); j++)
//		{
//			if (services.GetAttribName(j) == _T("name"))
//				name = services.GetAttribVal(j);
//			else if (services.GetAttribName(j) == _T("action"))
//				action =services.GetAttribVal(j);
//			else if (services.GetAttribName(j) == _T("appliesto"))
//				appliesto = services.GetAttribVal(j);
//			else if (services.GetAttribName(j) == _T("creator"))
//				creator = services.GetAttribVal(j);
//			else
//				ASSERT(0);//Error in XML
//		}
//		if (!name.IsEmpty() && !action.IsEmpty())
//		{
//			VERIFY(Register(name, action, appliesto, creator));
//		}
//	}
//	return TRUE;
//}


BOOL ServicesRegistrationSrv::ImportXMLFile(LPCTSTR wsxmlPath)
{
	TRACE(_T("ServicesRegistrationSrv::ImportXMLFile: %s\r\n"), wsxmlPath);
	CComPtr<IStream> pFileStream;
	CComPtr<IXmlReader> pReader;
	HRESULT hr;
	if (FAILED(FileStream::OpenFile(wsxmlPath, &pFileStream, false)))
		return FALSE;
	if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL)))
		return FALSE;
	if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)))
		return FALSE;
	if (FAILED(hr = pReader->SetInput(pFileStream)))
		return FALSE;
	XmlLiteReaderHelper hlp(*pReader);
	INT curDepth = hlp.GetCurrentDepth();
	if (hlp.FindElement(_T("services"), 0))
	{
		std::tstring name, action, appliesto, creator;
		while (hlp.FindElement(_T("service"), 1))
		{
			ServiceInfo si;
			LPCTSTR val = NULL;
			//=== Required
			if ((val = hlp.GetElementAttribute(_T("name"))) == NULL)
				continue;
			name = val;
			if ((val = hlp.GetElementAttribute(_T("action"))) == NULL)
				continue;
			action = val;
			if ((val = hlp.GetElementAttribute(_T("appliesto"))) == NULL)
				continue;
			appliesto = val;

			si.Name = name.c_str();
			si.Action = action.c_str();
			si.AppliesTo = appliesto.c_str();
			
			//=== Optional
			if ((val = hlp.GetElementAttribute(_T("creator"))) != NULL)
			{
				creator = val;
				si.Creator = creator.c_str();
			}
			if ((val = hlp.GetElementAttribute(_T("icon"))) != NULL)
			{
				si.hIcon = IconSerialization::LoadIcon(CT2CA(val));
				ASSERT(si.hIcon != NULL);
				TRACE(_T("ServicesRegistrationSrv::ImportXMLFile Loading icon from resources: %d\r\n"), si.hIcon ? 1 : 0);
			}
			if (si.hIcon == NULL)
			{
				TCHAR bf[MAX_PATH];
				_tcsncpy(bf, wsxmlPath, MAX_PATH);
				LPTSTR pos = _tcsrchr(bf, _T('.'));
				if (pos != NULL)
				{
					pos[1] = 0;
					_tcscat(bf, _T("ico"));
					si.hIcon = (HICON) LoadImage(NULL, bf, IMAGE_ICON, 16, 16, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
				}
			}
			VERIFY(AddService(si));
		}
	}
	return TRUE;
}


INT ServicesRegistrationSrv::LoadServices(LPCTSTR webServicesRoot)
{
	TRACEST(_T("ServicesRegistrationSrv::LoadServices"));
	ASSERT(webServicesRoot != NULL);
	ASSERT(webServicesRoot[_tcslen(webServicesRoot) - 1] == '\\');
	TCHAR spec[_MAX_PATH];
	_sntprintf(spec, _MAX_PATH, _T("%s*.wsxml"), webServicesRoot);
	WIN32_FIND_DATA data;
	HANDLE h=FindFirstFile(spec,&data);
	INT nImportedFiles = 0;
	if (h!=INVALID_HANDLE_VALUE) 
	{
		do 
		{
			TCHAR fname[_MAX_PATH];
			_sntprintf(fname, _MAX_PATH, _T("%s%s"), webServicesRoot, data.cFileName);
			if (ImportXMLFile(fname))
				nImportedFiles++;
		} 
		while (FindNextFile(h,&data));
		FindClose(h);
	}
	TRACE(_T("ServicesRegistrationSrv::LoadServices %d files imported containing %d services\r\n"), nImportedFiles, GetServicesCount());

	return nImportedFiles;
}

void ServicesRegistrationSrv::UnLoadServices()
{
	std::vector<ServiceInfoStorage>::iterator it = m_services.begin();
	for (; it != m_services.end(); it++)
		DestroyIcon((*it).hIcon);
	m_services.clear();
}


