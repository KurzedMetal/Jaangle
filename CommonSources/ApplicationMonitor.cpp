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
#include "ApplicationMonitor.h"
//#include "Registry.h"



#ifdef _UNITTESTING
BOOL ApplicationMonitor::UnitTest()
{
	TRACE(_T("===> ApplicationMonitor::UnitTest\r\n"));
	LPCTSTR hkey = _T("Software\\AppMon\\Test3");
	SHDeleteKey(HKEY_CURRENT_USER, hkey);
	UINT apUniqueID = 0;
	SYSTEMTIME installationDate;
	{
		ApplicationMonitor ap(FALSE);
		ap.Init(hkey, HKEY_CURRENT_USER);
		ap.EnableCrashDetector(TRUE);
		UNITTEST(ap.GetCount(CT_Runs) == 0);
		UNITTEST(ap.GetCount(CT_TotalRuns) == 0);
		ap.IncreaseAppCount();
		UNITTEST(ap.GetCount(CT_Runs) == 1);
		UNITTEST(ap.GetCount(CT_TotalRuns) == 1);
		UNITTEST(ap.GetCount(CT_Crashes) == 0);
		UNITTEST(ap.GetCount(CT_TotalCrashes) == 0);
		UNITTEST(ap.IsAfterCrash() == FALSE);
		apUniqueID = ap.GetAppUniqueID();
		UNITTEST(apUniqueID != 0);
		ap.GetInstallationDate(&installationDate);
		UNITTEST(installationDate.wYear > 0);
		UNITTEST(ap.GetRunningTimeInMinutes() == 0);
	}
	{
		ApplicationMonitor ap(FALSE);
		ap.Init(hkey, HKEY_CURRENT_USER);
		ap.IncreaseAppCount();
		ap.EnableCrashDetector(TRUE);
		UNITTEST(ap.GetCount(CT_Runs) == 2);
		UNITTEST(ap.GetCount(CT_TotalRuns) == 2);
		UNITTEST(ap.GetCount(CT_Crashes) == 0);
		UNITTEST(ap.GetCount(CT_TotalCrashes) == 0);
		UNITTEST(ap.IsAfterCrash() == FALSE);
		UNITTEST(apUniqueID == ap.GetAppUniqueID());
		SYSTEMTIME st;
		ap.GetInstallationDate(&st);
		UNITTEST(memcmp(&installationDate, &st, sizeof(SYSTEMTIME)) == 0);
		ap.SetCount(CT_Runs, 1);
		Sleep(1100);
		UNITTEST(ap.GetInstalledTime() > 0.0);
		UNITTEST(ap.GetInstalledTime() < 1.0);

		ApplicationMonitor ap2(FALSE);
		ap2.Init(hkey, HKEY_CURRENT_USER);
		ap2.IncreaseAppCount();
		UNITTEST(ap2.GetCount(CT_Runs) == 2);
		UNITTEST(ap2.GetCount(CT_TotalRuns) == 3);
		UNITTEST(ap2.IsAfterCrash() == TRUE);
		ap2.IncreaseCrashCount();
		UNITTEST(ap2.GetCount(CT_Crashes) == 1);
		UNITTEST(ap2.GetCount(CT_TotalCrashes) == 1);
	}
	SHDeleteKey(HKEY_CURRENT_USER, hkey);
	return TRUE;
}
#endif


LPCTSTR sAppMonRunning = _T("Running");
LPCTSTR sAppMonAppCount = _T("AppCount");
LPCTSTR sAppMonAppCountTotal = _T("AppCountTotal");
LPCTSTR sAppMonCrashCount = _T("CrashCount");
LPCTSTR sAppMonCrashCountTotal = _T("CrashCountTotal");
LPCTSTR sAppMonUniqueID = _T("UniqueID");
LPCTSTR sAppMonInstallationDate = _T("InstallationDate");
LPCTSTR sAppMonRunningTime = _T("RunningTime");
//LPCTSTR sAppMonInstalDate2 = _T("InstalDate");


ApplicationMonitor::ApplicationMonitor(BOOL bReadOnlyMode):
m_hKey(NULL),
m_appUniqueID(0),
m_bIsAfterCrash(FALSE),
m_runningTime(0),
m_bReadOnlyMode(bReadOnlyMode),
m_bStateChanged(FALSE)
{
	memset(m_count, 0, sizeof(m_count));
	memset(&m_installationDate, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&m_startTime);
}

ApplicationMonitor::~ApplicationMonitor()
{
	if (m_hKey != NULL)
	{
		if (m_bStateChanged == FALSE)
			SaveState();
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

BOOL ApplicationMonitor::WriteUINT(LPCTSTR keyName, UINT val)
{
	if (m_bReadOnlyMode)
		return TRUE;
	if (m_hKey == 0)
		return FALSE;
	TCHAR bf[50];
	_itot(val, bf, 10);
	if (RegSetValueEx(m_hKey, keyName, 0, REG_SZ, (LPBYTE)bf, _tcslen(bf) * sizeof(TCHAR)) == ERROR_SUCCESS)
		return TRUE;
	TRACE(_T("@1 ApplicationMonitor::WriteUINT Failure '%s'\r\n"), keyName);
	return FALSE;
}
BOOL ApplicationMonitor::ReadUINT(LPCTSTR keyName, UINT& val, UINT defVal)
{
	val = defVal;
	if (m_hKey == 0)
		return FALSE;
	DWORD type;
	DWORD dataSize = 50 * sizeof(TCHAR);
	TCHAR bf[50];
	if (RegQueryValueEx(m_hKey, keyName, 0, &type, (LPBYTE)bf, &dataSize) == ERROR_SUCCESS)
	{
		if (type == REG_SZ)
		{
			LPTSTR end = NULL;
			val = _tcstoul(bf, &end, 10);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL ApplicationMonitor::WriteSYSTEMTIME(LPCTSTR keyName, SYSTEMTIME val)
{
	if (m_bReadOnlyMode)
		return TRUE;
	if (m_hKey == 0)
		return FALSE;
	if (RegSetValueEx(m_hKey, keyName, 0, REG_BINARY, (LPBYTE)&val, sizeof(SYSTEMTIME)) == ERROR_SUCCESS)
		return TRUE;
	TRACE(_T("@1 ApplicationMonitor::WriteSYSTEMTIME Failure '%s'\r\n"), keyName);
	return FALSE;
}
BOOL ApplicationMonitor::ReadSYSTEMTIME(LPCTSTR keyName, SYSTEMTIME& val)
{
	if (m_hKey == 0)
		return FALSE;
	DWORD type;
	DWORD dataSize = sizeof(SYSTEMTIME);
	if (RegQueryValueEx(m_hKey, keyName, 0, &type, (LPBYTE)&val, &dataSize) == ERROR_SUCCESS)
	{
		if (type == REG_BINARY)
			return TRUE;
	}
	return FALSE;
}



void ApplicationMonitor::SaveState()
{
	if (m_hKey != NULL)
	{
		EnableCrashDetector(FALSE);
		DOUBLE tfn = GetTimeFromNow(&m_startTime);
		UINT sessionMinutes = UINT(tfn * 60 * 24);
		WriteUINT(sAppMonRunningTime, m_runningTime + sessionMinutes);
		RegFlushKey(m_hKey);
		m_bStateChanged = TRUE;
	}
	else
	{
		TRACE(_T("@1 ApplicationMonitor::SaveState m_hKey=NULL\r\n"));
	}
}

BOOL ApplicationMonitor::InitByName(LPCTSTR applicationName)
{
	TCHAR regKey[500];
	_sntprintf(regKey, 500, _T("Software\\%s\\AppMonDebug"), applicationName);
	return Init(regKey, HKEY_CURRENT_USER);
}



BOOL ApplicationMonitor::Init(LPCTSTR regKey, HKEY hKey/* = HKEY_CURRENT_USER*/)
{
	if (m_hKey != NULL)
		return FALSE;
	DWORD dwDisposition = 0;
	if (::RegCreateKeyEx(hKey, regKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey,
		&dwDisposition) != ERROR_SUCCESS)
	{
		TRACE(_T("@1 ApplicationMonitor::Init Cannot open registry\r\n"));
		return FALSE;
	}
	ReadUINT(sAppMonAppCount, m_count[CT_Runs], 0);
	ReadUINT(sAppMonAppCountTotal, m_count[CT_TotalRuns], 0);
	ReadUINT(sAppMonCrashCount, m_count[CT_Crashes], 0);
	ReadUINT(sAppMonCrashCountTotal, m_count[CT_TotalCrashes], 0);
	ReadUINT(sAppMonRunningTime, m_runningTime, 0);
	UINT val;
	ReadUINT(sAppMonRunning, val, 0);
	m_bIsAfterCrash = val != 0;
	ReadUINT(sAppMonUniqueID, m_appUniqueID, 0);
	ReadSYSTEMTIME(sAppMonInstallationDate, m_installationDate);

	if (m_appUniqueID == 0)
	{
		GetLocalTime(&m_installationDate);
		m_appUniqueID = MAKELPARAM(m_installationDate.wMilliseconds + m_installationDate.wSecond * 1000, GetTickCount());
		WriteUINT(sAppMonUniqueID, m_appUniqueID);
		WriteSYSTEMTIME(sAppMonInstallationDate, m_installationDate);
	}

	//reg[sAppMonAppCount] = m_appCount;
	//reg[sAppMonAppCountTotal] = m_totalAppCount;
	return TRUE;
}


void ApplicationMonitor::GetInstallationDate(SYSTEMTIME* st) const
{
	memcpy(st, &m_installationDate, sizeof(SYSTEMTIME));
}

DOUBLE ApplicationMonitor::GetInstalledTime() const
{
	return GetTimeFromNow(&m_installationDate);
}


DOUBLE ApplicationMonitor::GetTimeFromNow(const SYSTEMTIME* time)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	DOUBLE curTime = 0.0;
	SystemTimeToVariantTime(&st, &curTime);
	DOUBLE instTime = 0.0;
	SystemTimeToVariantTime((LPSYSTEMTIME)time, &instTime);
	return curTime - instTime;
}

UINT ApplicationMonitor::GetCount(CountType ct) const
{
	ASSERT(ct > CT_First && ct < CT_Last);
	if (ct > CT_First && ct < CT_Last)
		return m_count[ct];
	return 0;
}
void ApplicationMonitor::SetCount(CountType ct, UINT value)
{
	ASSERT(ct > CT_First && ct < CT_Last);
	if (ct > CT_First && ct < CT_Last)
	{
		m_count[ct] = value;
		switch (ct)
		{
		case CT_Crashes:
			WriteUINT(sAppMonCrashCount, value);
			break;
		case CT_Runs:
			WriteUINT(sAppMonAppCount, value);
			break;
		case CT_TotalCrashes:
			WriteUINT(sAppMonCrashCountTotal, value);
			break;
		case CT_TotalRuns:
			WriteUINT(sAppMonAppCountTotal, value);
			break;
		}
	}
}



//void ApplicationMonitor::ResetCrashCounter()
//{
//	m_bIsAfterCrash = FALSE;
//	m_crashCount = 0;
//	WriteUINT(sAppMonCrashCount, 0);
//}
//void ApplicationMonitor::ResetAppCounter()
//{
//	m_bIsAfterCrash = FALSE;
//	m_appCount = 1;
//	WriteUINT(sAppMonAppCount, 1);
//}

void ApplicationMonitor::IncreaseCrashCount()
{
	SetCount(CT_Crashes, GetCount(CT_Crashes) + 1);
	SetCount(CT_TotalCrashes, GetCount(CT_TotalCrashes) + 1);
}
void ApplicationMonitor::IncreaseAppCount()
{
	SetCount(CT_Runs, GetCount(CT_Runs) + 1);
	SetCount(CT_TotalRuns, GetCount(CT_TotalRuns) + 1);
}

void ApplicationMonitor::EnableCrashDetector(BOOL bEnable)
{
	WriteUINT(sAppMonRunning, bEnable ? 1 : 0);
}




