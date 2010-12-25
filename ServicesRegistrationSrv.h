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
#ifndef _ServicesRegistrationSrv_h_
#define _ServicesRegistrationSrv_h_


enum ServiceTypeEnum
{
	STYPE_Unknown,
	STYPE_ArtistInfo,
	STYPE_AlbumInfo,
	STYPE_TrackInfo,
	STYPE_Last
};
class ServicesExecuteInfo
{
public:
	ServicesExecuteInfo():artist(NULL),album(NULL),title(NULL)	{}
	LPCTSTR artist;
	LPCTSTR album;
	LPCTSTR title;
};

class ServiceInfo
{
public:
	ServiceInfo()	{memset(this, 0, sizeof(ServiceInfo));}
	LPCTSTR Name;
	LPCTSTR Action;
	LPCTSTR Creator;
	LPCTSTR AppliesTo;
	HICON hIcon;
	BOOL CanHandle(ServiceTypeEnum type);
};

class ServicesRegistrationSrv
{
public:
	class IWebViewer
	{
	public:
		virtual BOOL OpenURL(LPCTSTR path)					= 0;
	};
public:
	ServicesRegistrationSrv():m_pWebViewer(NULL)			{}
	~ServicesRegistrationSrv()								{UnLoadServices();}

	
	INT LoadServices(LPCTSTR webServicesRoot);
	void UnLoadServices();
	UINT GetServicesCount();

	BOOL GetServiceInfo(UINT ID, ServiceInfo& serviceInfo);

	BOOL Execute(ServiceInfo& service, ServicesExecuteInfo& inf, IWebViewer* pWebViewer);


	void SetWebViewer(IWebViewer* pWebViewer)		{m_pWebViewer = pWebViewer;}
	IWebViewer* GetWebViewer()						{return m_pWebViewer;}


private:
	BOOL AddService(ServiceInfo si);
	BOOL ImportXMLFile(LPCTSTR FilePath);
	struct ServiceInfoStorage
	{
		ServiceInfoStorage():hIcon(NULL)	{}
		std::tstring Name;
		std::tstring Action;
		std::tstring Creator;
		std::tstring AppliesTo;
		HICON hIcon;
	};
	std::vector<ServiceInfoStorage> m_services;
	IWebViewer* m_pWebViewer;
};


#endif
