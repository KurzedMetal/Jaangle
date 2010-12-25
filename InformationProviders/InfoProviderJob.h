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
#ifndef _InformationProviderJob_h_
#define _InformationProviderJob_h_

#include "MultiThreadJobQueue.h"
#include "InformationProviders/InfoProvider.h"

class InfoProviderContainer;

class InfoProviderJob: public Job
{
public:
	virtual ~InfoProviderJob()										
	{
		delete m_pIP;
	}

	virtual State GetState()							{return m_state;}
	virtual void Execute()
	{
		if (m_pIP != NULL)
		{
			m_bAbort = FALSE;
			m_state = ST_Running;
			m_bLastResult = m_pIP->GetNextResult(m_result);
			m_state = m_bAbort ? ST_Aborted : ST_Finished;
			m_bAbort = FALSE;
		}
	}
	virtual void Abort()								{m_bAbort = TRUE;}

	//const IInfoProvider* GetInfoProvider()				{return m_pIP;}
	BOOL GetLastResult(IInfoProvider::Result& result)
	{
		ASSERT(m_state != ST_Running);
		if (m_state == ST_Finished)
		{
			result = m_result;
			return m_bLastResult;
		}
		return FALSE;
	}
	BOOL GetLastRequest(IInfoProvider::Request& request)
	{
		ASSERT(m_state != ST_Running);
		if (m_state != ST_Running)
		{
			request = m_pIP->GetRequest();
			return TRUE;
		}
		return FALSE;
	}

	UINT GetItemID()									{return m_ID;}


	static InfoProviderJob* CreateInfoProviderJob(IInfoProvider* pIP, UINT itemID)
	{
		ASSERT(pIP != NULL);
		InfoProviderJob* pIPJ = new InfoProviderJob;
		pIPJ->SetInfoProvider(pIP);
		pIPJ->m_ID = itemID;
		return pIPJ;
	}

private:
	InfoProviderJob():m_pIP(NULL),m_bAbort(FALSE),m_bLastResult(FALSE)		{}
	void SetInfoProvider(IInfoProvider* pIP)		
	{
		delete m_pIP;//Deletes the old IP
		m_pIP = pIP;
	}

	IInfoProvider* m_pIP;
	State m_state;
	BOOL m_bAbort;
	UINT m_ID;
	IInfoProvider::Result m_result;
	BOOL m_bLastResult;
};


#endif
