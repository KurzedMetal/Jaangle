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
#include "MultiTracker.h"

#include "MSNAdapterTracker.h"
#include "LocalHistoryTracker.h"
#include "TSOnlineAdapterTracker.h"
#include "LastFMAdapterTracker.h"

#include "PrgAPI.h"



MultiTracker::MultiTracker():
m_pMSN(NULL),
m_pOH(NULL),
m_pLH(NULL),
m_pLFM(NULL)
{
}

MultiTracker::~MultiTracker(void)			
{
	delete m_pLFM;
	delete m_pLH;
	delete m_pMSN;
	delete m_pOH;
}

void MultiTracker::SubmitTrack(Action action, const TrackerTrack& track, UINT actionTime)
{
	TRACEST(_T("MultiTracker::SubmitTrack"));
	PrgAPI* pAPI = PRGAPI();
	if (pAPI->GetOption(OPT_GEN_LastFMScrobbler))
	{
		if (m_pLFM == NULL)
		{
			m_pLFM = new LastFMAdapterTracker;
			m_pLFM->SetLastFM(pAPI->GetLastFMServices());
		}
		m_pLFM->SubmitTrack(action, track, actionTime);
	}
	if (pAPI->GetOption(OPT_GEN_NotifyMSN))
	{
		if (m_pMSN == NULL)
			m_pMSN = new MSNAdapterTracker;
		m_pMSN->SubmitTrack(action, track, actionTime);
	}
	if (pAPI->GetOption(OPT_GEN_NotifyHistory))
	{
		if (m_pLH == NULL)
		{
			m_pLH = new LocalHistoryTracker;
			m_pLH->SetSQLManager(pAPI->GetSQLManager());
		}
		m_pLH->SubmitTrack(action, track, actionTime);
	}
	if (pAPI->GetOption(OPT_GEN_UseNativeOnlineServices))
	{
		if (m_pOH == NULL)
		{
			m_pOH = new TSOnlineAdapterTracker;
			m_pOH->SetTSOnlineServices(pAPI->GetTSOnlineServices());
		}
		m_pOH->SubmitTrack(action, track, actionTime);
	}
}
