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
#include "AppMonitor.h"
#include "atlenc.h"

#ifdef _UNITTESTING
BOOL AppMonitor::UnitTest()
{
	TRACE(_T("===> AppMonitor::UnitTest\r\n"));
	TCHAR state[500];
	{
		AppMonitor ap;
		ap.StartMonitoring();
		ap.IncreaseAppCount();
		ap.IncreaseAppCount();
		ap.IncreaseCrashCount();
		UNITTEST(ap.GetState().Runs == 2);
		UNITTEST(ap.GetState().TotalRuns == 2);
		UNITTEST(ap.GetState().Crashes == 1);
		UNITTEST(ap.GetState().TotalCrashes == 1);
		UNITTEST(ap.IsAfterCrash() == FALSE);
		UNITTEST(ap.GetState().AppUniqueID != 0);
		UNITTEST(ap.GetState().InstallationDate.wYear > 0);
		UNITTEST(ap.GetState().RunningTimeInMinutes == 0);
		ap.StopMonitoring();

		UNITTEST(ap.Save(state, 500));
	}
	{
		AppMonitor ap;
		ap.Load(state);
		ap.StartMonitoring();
		UNITTEST(ap.GetState().Runs == 2);
		UNITTEST(ap.GetState().TotalRuns == 2);
		UNITTEST(ap.GetState().Crashes == 1);
		UNITTEST(ap.GetState().TotalCrashes == 1);
		UNITTEST(ap.IsAfterCrash() == FALSE);
		UNITTEST(ap.GetState().AppUniqueID != 0);
		UNITTEST(ap.GetState().InstallationDate.wYear > 0);
		UNITTEST(ap.GetState().RunningTimeInMinutes == 0);
	
		UNITTEST(ap.Save(state, 500));
	}
	{
		AppMonitor ap;
		ap.Load(state);
		ap.StartMonitoring();
		UNITTEST(ap.IsAfterCrash() == TRUE);
	}



	return TRUE;
}
#endif




AppMonitor::AppMonitor():
m_bIsAfterCrash(FALSE)
{
	memset(&m_startTime, 0, sizeof(SYSTEMTIME));
}

AppMonitor::~AppMonitor()
{
}
//GetLocalTime(&m_startTime);


//=== Start Monitoring 
//		1. Resets the RunningTime Counter
//		2. Sets the bRunning to TRUE
//		3. If the AppUniqueID is 0 then it sets the InstallationDate and creates a new UID
//	Usually use it after Load(...)
void AppMonitor::StartMonitoring()
{
	GetLocalTime(&m_startTime);
	m_state.bRunning = TRUE;

	if (m_state.AppUniqueID == 0)
	{
		GetLocalTime(&m_state.InstallationDate);
		m_state.AppUniqueID = MAKELPARAM(m_state.InstallationDate.wMilliseconds + m_state.InstallationDate.wSecond * 1000, GetTickCount());
	}


}
//=== Start Monitoring 
//		1. Updates the "Running Minutes" state value
//		2. Sets the bRunning to FALSE
//	Usually use it before Save(...)
void AppMonitor::StopMonitoring()
{
	m_state.RunningTimeInMinutes = GetRunningTimeInMinutes();
	m_state.bRunning = FALSE;
}


//		1. Calculates The IsAfterCrash Flag from the bRunning Flag
BOOL AppMonitor::Load(LPCTSTR stateString)
{

	CHAR bf[2 * sizeof(State)];
	INT i = 0;
	for (; i < 2 * sizeof(State); i++)
	{
		bf[i] = (CHAR)stateString[i];
		if (stateString[i] == 0)
			break;
	}


	State tmpState;
	INT len = sizeof(State);
	if (!Base64Decode(bf, i, (BYTE*)&tmpState, &len))
		return FALSE;
	if (len != sizeof(State))
		return FALSE;




	//BYTE b[sizeof(State)];
	//TCHAR tmp[3];
	//tmp[2] = 0;
	//for (INT i = 0; i < sizeof(State); i++)
	//{
	//	tmp[0] = stateString[2*i];
	//	tmp[1] = stateString[2*i+1];
	//	if (tmp[0] == 0 || tmp[1] == 0)
	//		return FALSE;
	//	LPTSTR endPtr = 0;
	//	b[i] = (BYTE)_tcstoul(tmp, &endPtr, 16);
	//}
	//State* pState = (State*) &b;
	//===Make some sane tests here.
	if (tmpState.InstallationDate.wYear < 2000)
		return FALSE;
	if (tmpState.InstallationDate.wHour >= 24)
		return FALSE;
	if (tmpState.InstallationDate.wMinute >= 60)
		return FALSE;
	if (tmpState.InstallationDate.wSecond >= 60)
		return FALSE;



	m_state = tmpState;
	m_bIsAfterCrash = (m_state.bRunning == TRUE);
	return TRUE;
}

BOOL AppMonitor::Save(LPTSTR bf, UINT bfLen)
{
	if (bfLen < ((sizeof(State) * 2) + 1))
		return FALSE;

	CHAR base64Data[2 * sizeof(State)];
	INT base64DataLen = 2 * sizeof(State);
	if (!Base64Encode((BYTE*)&m_state, sizeof(State), base64Data, &base64DataLen, ATL_BASE64_FLAG_NOCRLF))
		return FALSE;
	INT i = 0;
	for (; i < base64DataLen; i++)
		bf[i] = base64Data[i];
	bf[i] = 0;
	return TRUE;

	//BYTE* pState = (BYTE*) &m_state;
	//for (INT i = 0; i < sizeof(State); i++)
	//	_sntprintf(&bf[i*2], 3, _T("%02X"), pState[i]);
	//return TRUE;
}

DOUBLE AppMonitor::GetInstalledTime() const
{
	return GetTimeFromNow(&m_state.InstallationDate);
}

UINT AppMonitor::GetRunningTimeInMinutes() const
{
	if (m_startTime.wYear == 0)
		return m_state.RunningTimeInMinutes;
	return m_state.RunningTimeInMinutes + INT(GetTimeFromNow(&m_startTime) / 60);
}



DOUBLE AppMonitor::GetTimeFromNow(const SYSTEMTIME* time)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	DOUBLE curTime = 0.0;
	SystemTimeToVariantTime(&st, &curTime);
	DOUBLE instTime = 0.0;
	SystemTimeToVariantTime((LPSYSTEMTIME)time, &instTime);
	return curTime - instTime;
}

void AppMonitor::IncreaseCrashCount()
{
	m_state.Crashes++;
	m_state.TotalCrashes++;
}
void AppMonitor::IncreaseAppCount()
{
	m_state.Runs++;
	m_state.TotalRuns++;
}

