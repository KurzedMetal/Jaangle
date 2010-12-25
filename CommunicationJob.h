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
#ifndef _CommunicationJobJob_h_
#define _CommunicationJobJob_h_

#include "MultiThreadJobQueue.h"
#include <WinInet.h>

class CommunicationJob: public Job
{
public:
	virtual ~CommunicationJob();

	virtual State GetState()									{return m_state;}
	virtual void Execute();
	virtual void Abort()										{}


	void SetHINTERNET(HINTERNET hNet)							{m_hNet = hNet;}
	static CommunicationJob* CreateJob(LPCTSTR url, LPCSTR data, HINTERNET hNet)
	{
		CommunicationJob* pJob = new CommunicationJob(url, data);
		pJob->SetHINTERNET(hNet);
		return pJob;
	}

private:
	CommunicationJob(LPCTSTR url, LPCSTR data):m_url(url),m_data(data),m_hNet(NULL)
	{}
	std::basic_string<TCHAR> m_url;
	std::basic_string<CHAR> m_data;
	State m_state;
	HINTERNET m_hNet;


};


#endif
