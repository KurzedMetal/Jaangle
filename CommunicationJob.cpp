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
#include "CommunicationJob.h"
#include "WebPageUtilities.h"

CommunicationJob::~CommunicationJob()
{
}

void CommunicationJob::Execute()
{
	m_state = ST_Running;
	std::string page;
	//m_url = _T("http://services.artificialspirit.com/tsservice.php");
	//TRACE(_T("@3 CommunicationJob::Execute\r\n"));
	TRACEST(_T("CommunicationJob::Execute"), m_url.c_str());
	if (m_hNet)
	{
		if (m_data.empty())
		{
			if (DownloadWebPage(page, m_hNet, m_url.c_str()))
			{
				if (page != "OK")
					TRACE(_T("@1 CommunicationJob::Execute RESP: '%s'"), CA2CT(page.c_str()));
			}
		}
		else
		{
			if (DownloadWebPagePost(page, m_hNet, m_url.c_str(), m_data.c_str()))
			{
				if (page != "OK")
					TRACE(_T("@1 CommunicationJob::Execute RESP: '%s'"), CA2CT(page.c_str()));
			}

		}
	}
	else
		TRACE(_T("@1 CommunicationJob::Execute hNet == NULL\r\n"));

	m_state = ST_Finished;
}


