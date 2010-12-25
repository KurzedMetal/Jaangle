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
#include "ALog.h"
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")


#ifdef USE_CPUTICKER
#include "cputicker.h"
#endif


#ifdef ALOG_ACTIVE
	#pragma message("********* ALOG IS ACTIVE **************")
	ALogAutoShurdownHelper inst;//Auto ShutDown
#else
	#pragma message("********* ALOG DISABLED *********")
#endif

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//CCritSection
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

CCritSection::CCritSection()
{
	::InitializeCriticalSectionAndSpinCount(&m_CS, 0x80000400);
}
CCritSection::~CCritSection()
{
	DeleteCriticalSection(&m_CS);
}

void CCritSection::Lock()
{
	EnterCriticalSection(&m_CS);
}

void CCritSection::UnLock()
{
	LeaveCriticalSection(&m_CS);
}

BOOL CCritSection::IsLocked()
{
	return TryEnterCriticalSection(&m_CS) != 0;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//CLock
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
CThreadLock::CThreadLock(CCritSection* cs, bool InitialLock):
m_cs(NULL)
{
	ASSERT(cs != NULL);
	m_cs = cs;
	if (InitialLock)
		Lock();
}
CThreadLock::~CThreadLock()
{
	ASSERT(m_cs != NULL);
	m_cs->UnLock();
}
void CThreadLock::Lock()
{
	ASSERT(m_cs != NULL);
	m_cs->Lock();
}




//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//ALog
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

ALog* ALog::m_pLog = NULL;

ALog::ALog():
m_pViewer(NULL),
m_pTicker(NULL),
m_MainThreadID(NULL),
m_StartupTicks(NULL),
m_markedTime(0),
m_markedMemSize(0)
{
	SetOption(LO_MaxAllowedDepth, 10);
	SetOption(LO_DefaultDepth, 3);
	SetOption(LO_ShowDepth, 1);
	SetOption(LO_ShowTimeFromStartup, 0);
	SetOption(LO_ShowThreadID, 1);
	SetOption(LO_ShowTimeDate, 0);
	SetOption(LO_AlwaysCheckForLastError, 0);

}


ALog::~ALog()	
{
	if (m_pViewer != NULL)
	{
		SYSTEMTIME t;
		::GetLocalTime(&t);
		TCHAR bf[200];
		_sntprintf(bf, 200, _T("@@ ------End Logging: %d-%d-%d %0.2d:%0.2d:%0.2d .%0.3dms\r\n"),
			t.wDay, t.wMonth, t.wYear,   t.wHour, t.wMinute, t.wSecond,  t.wMilliseconds);
		Log(bf);
		delete m_pViewer;
		m_pViewer = NULL;
	}
#ifdef USE_CPUTICKER
	delete m_pTicker;
#endif
}

void ALog::Shutdown()
{
	if (m_pLog)
	{
		delete m_pLog;
		m_pLog = NULL;
	}
}


ALog* ALog::Inst()
{
	if (m_pLog == NULL)
	{
		m_pLog = new ALog;
		m_pLog->m_StartupTicks = GetTickCount();
		m_pLog->m_MainThreadID = ::GetCurrentThreadId();
	}
	return m_pLog;
}
void ALog::AddViewer(AViewer* viewer)
{
	ASSERT(viewer != NULL);
	BOOL bFirstTime = FALSE;
	if (m_pViewer == NULL)
	{
		bFirstTime = TRUE;
		m_pViewer = new MultiViewer;
	}
	m_pViewer->AddViewer(viewer);
	if (bFirstTime)
	{
		SYSTEMTIME t;
		::GetLocalTime(&t);
		TCHAR bf[300];
		_sntprintf(bf, 200, _T("@@ ------Start Logging: %d-%d-%d %0.2d:%0.2d:%0.2d .%0.3dms\r\n"),
			t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond,  t.wMilliseconds);
		Log(bf);
		OSVERSIONINFO osVer;
		osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osVer);
		_sntprintf(bf, 200, _T("@@ VersionInfo: %d.%d.%d\r\n"), osVer.dwMajorVersion, osVer.dwMinorVersion, osVer.dwBuildNumber);
		Log(bf);
		MEMORYSTATUS memStat;
		GlobalMemoryStatus (&memStat);
		Log(_T("@@ MemInfo: %d / %d (MB)\r\n"), memStat.dwAvailPhys/1024/1024, memStat.dwTotalPhys/1024/1024);
		GetWindowsDirectory(bf, MAX_PATH);
		Log(_T("@@ <WINDIR>: %s\r\n"), bf);
		Log(_T("@@ SysDefLangID: %d\r\n"), GetSystemDefaultLangID());
		Log(_T("@@ ---\r\n"));
	}
	else
		Log(_T("@@ New Viewer Added\r\n"));
}

AViewer* ALog::GetViewer(INT idx)
{

	return m_pViewer->GetViewer(idx);
}


void ALog::SetOption(LogOptionEnum id, INT val)
{
	ASSERT(id >= 0 && id < LO_Last);
	if (id >= 0 && id < LO_Last)
		m_option[id] = val;
}
INT ALog::GetOption(LogOptionEnum id)
{
	ASSERT(id >= 0 && id < LO_Last);
	if (id >= 0 && id < LO_Last)
		return m_option[id];
	return 0;
}

//File Comments
//"Viewer" may have the follow value
//	- 0: AutoFind the best viewer (AlogDebugger2 external app if available else Debugger else File)
//	- 1: File
//	- 2: Debugger
//	- 3: Internal Window
//  - Any other value disables logging
//"LogFilePath" has the name of the logging file
//"Depth" Allowed depth
//	- 0: Only Errors
//	- 1: + Warnings
//	- 2: + L1 Information
//	- 3: + L2 Information
//	- 4: Everything
//"ShowDepth"
//	- 0: Does not show the depth in the logging string
//	- 1: Shows it
//"ShowTimeFromStartup"
//	- 0, 1
//"ShowThreadID"
//	- 0: Never shows the thread ID
//	- 1: Shows it if is different than the "main" thread (The thread that the logger was created)
//"ShowTimeDate"
//	- 0, 1

const LPCTSTR section = _T("ALOG");

AViewer* ALog::GetViewerByID(INT viewerID, LPCTSTR iniFilePath)
{
	switch (viewerID)
	{
	case 0:
		return GetAutoViewer();
		break;
	case 1:
		{
			TCHAR logFilePath[MAX_PATH];
			GetPrivateProfileString(section, _T("LogFilePath"), _T("debug.log"), logFilePath, MAX_PATH, iniFilePath);
			return new FileViewer(logFilePath);
		}
		break;
	case 2:
		return new DebugViewer;
		break;
	case 3:
		return new AutoWindowViewer(AfxGetInstanceHandle());
		break;
	case 4:
#ifdef USE_PRUNNEDFILEVIEWER
		{
			TCHAR logFilePath[MAX_PATH];
			GetPrivateProfileString(section, _T("LogFilePath"), _T("debug.log"), logFilePath, MAX_PATH, iniFilePath);
			return new PrunnedFileViewer(logFilePath);
		}
#else
		return NULL;
#endif
		break;

#ifdef USE_MULTIVIEWER
	case 5:
		{
			MultiViewer* pMV = new MultiViewer(TRUE);
			INT viewers = 0;
			while (TRUE)
			{
				viewers++;
				TCHAR bf[50];
				_sntprintf(bf, 50, _T("Viewer_%d"), viewers);
				INT vID = GetPrivateProfileInt(section, bf, -1, iniFilePath);
				if (vID == -1 || vID == 5)
					break;
				AViewer* pV = GetViewerByID(vID, iniFilePath);
				if (pV != NULL)
					pMV->AddViewer(pV);
			}
			return pMV;
		}
		break;
#endif
	case 6:
		//return new (GetAutoViewer());
		break;
	}
	return NULL;
}


BOOL ALog::LoadConfiguration(LPCTSTR path)
{
	HANDLE h = ::CreateFile(path,GENERIC_READ,FILE_SHARE_READ, 
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (h == INVALID_HANDLE_VALUE)
		return FALSE;
	CloseHandle(h);
	INT viewerID = GetPrivateProfileInt(section, _T("Viewer"), 0, path);
	AddViewer(GetViewerByID(viewerID, path));

	SetOption(LO_MaxAllowedDepth, GetPrivateProfileInt(section, 
		_T("Depth"), GetOption(LO_ShowDepth), path));
	SetOption(LO_ShowDepth, GetPrivateProfileInt(section, 
		_T("ShowDepth"), GetOption(LO_ShowDepth), path));
	SetOption(LO_ShowTimeFromStartup, GetPrivateProfileInt(section, 
		_T("ShowTimeFromStartup"), GetOption(LO_ShowTimeFromStartup), path));
	SetOption(LO_ShowThreadID, GetPrivateProfileInt(section, 
		_T("ShowThreadID"), GetOption(LO_ShowThreadID), path));
	SetOption(LO_ShowTimeDate, GetPrivateProfileInt(section, 
		_T("ShowTimeDate"), GetOption(LO_ShowTimeDate), path));

	//Write values to the file (if does not already) exist
	TCHAR bf[30];
	WritePrivateProfileString(section, 
		_T("Viewer"), _itot(viewerID, bf, 10), path);
	WritePrivateProfileString(section, 
		_T("Depth"), _itot(GetOption(LO_MaxAllowedDepth), bf, 10), path);
	WritePrivateProfileString(section, 
		_T("ShowDepth"), _itot(GetOption(LO_ShowDepth), bf, 10), path);
	WritePrivateProfileString(section, 
		_T("ShowTimeFromStartup"), _itot(GetOption(LO_ShowTimeFromStartup), bf, 10), path);
	WritePrivateProfileString(section, 
		_T("ShowThreadID"), _itot(GetOption(LO_ShowThreadID), bf, 10), path);
	WritePrivateProfileString(section, 
		_T("ShowTimeDate"), _itot(GetOption(LO_ShowTimeDate), bf, 10), path);
	return TRUE;
}

const TCHAR* const Depth[] = 
{
	_T("DBG"),	//@@
	_T("SYS"),	//@D
	_T("ERR"),	//@0
	_T("WAR"),	//@1
	_T("-L1"),	//@2
	_T("-L2"),	//@3
	_T("-L3")	//@4
};

enum MessageDepth
{
	MD_Invalid = -10,
	MD_Debug = -2,		//@D
	MD_System,			//@@
	MD_Error,			//@0
	MD_Warning,			//@1
	MD_InfoL1,			//@2
	MD_InfoL2,			//@3
	MD_InfoL3,			//@4
	MD_Last
};

void ALog::Log(LPCTSTR description)
{
#ifdef _DEBUG
	LPCTSTR pos = _tcschr(description, '%');
	ASSERT(pos == NULL);
#endif
	LogUnsecure(description);
}
void ALog::Log(LPCTSTR description, LPCTSTR s1)
{
#ifdef _DEBUG
	LPCTSTR pos = _tcschr(description, '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 's' || pos[2] == 's' || pos[3] == 's' || pos[4] == 's' || pos[5] == 's' || pos[6] == 's');
	ASSERT(_tcschr(&pos[1], '%') == NULL);
#endif
	LogUnsecure(description, s1);
}
void ALog::Log(LPCTSTR description, INT i1)
{
#ifdef _DEBUG
	LPCTSTR pos = _tcschr(description, '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 'd' || pos[1] == 'u' || pos[1] == 'c' || pos[1] == 'x' || pos[1] == 'X');
	ASSERT(_tcschr(&pos[1], '%') == NULL);
#endif
	LogUnsecure(description, i1);
}
void ALog::Log(LPCTSTR description, LPCTSTR s1, LPCTSTR s2)
{
#ifdef _DEBUG
	LPCTSTR pos = _tcschr(description, '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 's');
	pos = _tcschr(&pos[1], '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 's');
	ASSERT(_tcschr(&pos[1], '%') == NULL);
#endif
	LogUnsecure(description, s1, s2);

}
void ALog::Log(LPCTSTR description, INT i1, INT i2)
{
#ifdef _DEBUG
	LPCTSTR pos = _tcschr(description, '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 'd' || pos[1] == 'c');
	pos = _tcschr(&pos[1], '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 'd' || pos[1] == 'c');
	ASSERT(_tcschr(&pos[1], '%') == NULL);
#endif
	LogUnsecure(description, i1, i2);

}
void ALog::Log(LPCTSTR description, LPCTSTR s1, INT i2)
{
#ifdef _DEBUG
	LPCTSTR pos = _tcschr(description, '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 's');
	pos = _tcschr(&pos[1], '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 'd');
	ASSERT(_tcschr(&pos[1], '%') == NULL);
#endif
	LogUnsecure(description, s1, i2);

}
void ALog::Log(LPCTSTR description, INT i1, LPCTSTR s2)
{
#ifdef _DEBUG
	LPCTSTR pos = _tcschr(description, '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 'd');
	pos = _tcschr(&pos[1], '%');
	ASSERT(pos != NULL);
	ASSERT(pos[1] == 's');
	ASSERT(_tcschr(&pos[1], '%') == NULL);
#endif
	LogUnsecure(description, i1, s2);

}

void ALog::LogHWND(LPCTSTR description, HWND hwnd)
{
	if (description == NULL || hwnd == NULL)
		return;
	TCHAR bf[1000];
	const size_t bfLen = 300;
	TCHAR clsName[bfLen];
	if (::GetClassName(hwnd, clsName, bfLen) == 0)
	{
		INT er = GetLastError();
		SetLastError(0);
		clsName[0] = 0;
	}
	TCHAR ttlName[bfLen];
	if (::GetWindowText(hwnd, ttlName, bfLen) == 0)
	{
		INT er = GetLastError();
		SetLastError(0);
		ttlName[0] = 0;
	}
	bf[0] = 0;
	_sntprintf(bf, 1000, _T("%s\r\n\tCLS:'%s'\r\n\tTXT:'%s'\r\n\thwnd:%d\r\n"), 
		description, clsName, ttlName, hwnd);
	bf[999] = 0;
	LogUnsecure(bf);
}


void ALog::LogUnsecure(LPCTSTR description, ...)
{
	if (m_pViewer == NULL)
		return ;
	INT messageDepth = MD_Invalid;
	LPCTSTR pos = description;
	DOUBLE measure = -1.0;
	//=== Retrieve the depth
	if (*pos == '@')
	{
		switch (pos[1])
		{
		case '0':	//=== GetLastError TRACE("@0 Class-Function. The error is: ", x, y, z); 
		case '1':	//=== GetLastError TRACE("@0 Class-Function. The warning is: ", x, y, z); 
		case '2':	//=== GetLastError TRACE("@0 Class-Function. The Info L1 is: ", x, y, z); 
		case '3':
		case '4':
			if (*pos == '0')
				messageDepth = MD_System;
			else
				messageDepth = pos[1] - '1' + 1;
			pos+=2;
			break;
		case '@':	//=== System Message.
			messageDepth = MD_System;
			pos+=2;
			break;
		case 'D':	//=== Debug Message.
			//Function: Exactly like warnings
			//TRACE("@D Removed on release");
			//WHY? Sometimes you want to debug a certain function and remove
			//the debug info later. You can easily search and remove them using 'TRACE(_T("@D' search
			messageDepth = MD_Debug;
			pos+=2;
			break;
		}
	}
	if (messageDepth == MD_Invalid)
		messageDepth = GetOption(LO_DefaultDepth);

	if (GetOption(LO_AlwaysCheckForLastError) && !(messageDepth == MD_System))
	{
		DWORD dwLastError = GetLastError();
		if (
			dwLastError == 2 ||	//"The system cannot find the file specified"
			dwLastError == 3 ||	//"The system cannot find the path specified"
			dwLastError == 6 || //"The handle is invalid"
			dwLastError == 18)	//"There are no more files"
		{
			SetLastError(0);
			dwLastError = 0;
		}
		if (dwLastError)
		{
			SetLastError(0);
			const int mLen = 1500;
			TCHAR message[mLen];
			message[0] = 0;
			LPTSTR lpMsgBuf;
			INT len = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, dwLastError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				(LPTSTR) &lpMsgBuf,	0, NULL );
			if (len>0)
			{
				if (len>2)
					lpMsgBuf[len - 3] = 0;
				_sntprintf(message, 1500, _T("@@ GetLastError [AUTO]: [%d] '%s'\r\n"), dwLastError, lpMsgBuf);
				LocalFree((HLOCAL)lpMsgBuf);
			}
			else
				_sntprintf(message, 1500, _T("@@ GetLastError [AUTO]: [%d] '?'\r\n"), dwLastError);
			SetLastError(0);
			Log(message);
		}
	}
	if (messageDepth > GetOption(LO_MaxAllowedDepth))
		return;//===------------------------------Filter OUT all messages

	const int mLen = 2500;
	TCHAR message[mLen];
	message[0] = 0;
	INT curLen = 0;
	if (GetOption(LO_ShowDepth))
		curLen += _sntprintf(&message[curLen], mLen - curLen, _T("[%s] "), Depth[messageDepth + 2]);
	DWORD dwLastError = GetLastError();
	if (dwLastError && GetOption(LO_AlwaysCheckForLastError))
	{
		LPTSTR lpMsgBuf;
		INT len = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, dwLastError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			(LPTSTR) &lpMsgBuf,	0, NULL );
		if (len>0)
		{
			if (len>2)
				lpMsgBuf[len - 3] = 0;
			curLen += _sntprintf(&message[curLen], mLen - curLen, _T("LastError: [%d] '%s'\r\n\t"), dwLastError, lpMsgBuf);
			LocalFree((HLOCAL)lpMsgBuf);
		}
		else
			curLen += _sntprintf(&message[curLen], mLen - curLen, _T("LastError: [%d] - No Description available\r\n\t"), dwLastError);
		SetLastError(0);
	}

	if (*pos == '@')
	{
		pos++;
		switch (*pos)
		{
		case 'E':	//=== GetLastError TRACE("@E Class-Function.: ", x, y, z); 
					//Also: TRACE("@1@E Class-Function.: ", x, y, z); 
					//If there is no error - no message will be displayed
			{
				pos++;
				LPTSTR lpMsgBuf;
				if (dwLastError == 0)
				{
					dwLastError = GetLastError();
					SetLastError(0);
				}
				if (dwLastError == 0)//No error
					return;
				INT len = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, dwLastError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
					(LPTSTR) &lpMsgBuf,	0, NULL );
				if (len>0)
				{
					if (len>2)
						lpMsgBuf[len - 3] = 0;
					curLen += _sntprintf(&message[curLen], mLen - curLen, _T("LastError: '%s'\r\n\t"), lpMsgBuf);
					LocalFree((HLOCAL)lpMsgBuf);
				}
				else
					curLen += _sntprintf(&message[curLen], mLen - curLen, _T("LastError: 'Unknown' : %d\r\n\t"), dwLastError);
			}
			break;
#ifdef USE_CPUTICKER
		case 'T':	//=== Mark - Measure Time
			//TRACE("@T"); ==> silent (no message). Just marks the time
			//TRACE("@T Class-Function.", x,y,z); ==> [M:Measured Time xxx.xx ms]
			//Also TRACE("@T") for silent (no message)
			{
				if (m_pTicker == NULL)
					m_pTicker = new CCPUTicker;
				__int64 curTime = m_pTicker->Measure();
				__int64 passedTime = curTime - m_markedTime;
				m_markedTime = curTime;
				pos++;
				if (*pos == 0)
					return;//Do NOT SHOW THIS MESSAGE
				curLen += _sntprintf(&message[curLen], mLen - curLen, _T("[T: %6.5f ms]"),
					(passedTime * 1000.0) / CCPUTicker::GetCachedCPUFrequency());
				message[mLen - 1] = 0;
			}
			break;
#endif
		case 'I':
			{
				pos++;
				HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ::GetCurrentProcessId());
				if(hProcess)
				{
					PROCESS_MEMORY_COUNTERS ProcessMemoryCounters;
					memset(&ProcessMemoryCounters, 0, sizeof(ProcessMemoryCounters));
					ProcessMemoryCounters.cb = sizeof(ProcessMemoryCounters);
					if(::GetProcessMemoryInfo(hProcess,	&ProcessMemoryCounters,	sizeof(ProcessMemoryCounters)) == TRUE)
					{
						curLen += _sntprintf(&message[curLen], 
							mLen - curLen, _T("[I:Memory: %d (%d) / %d (kb)] "), 
							ProcessMemoryCounters.WorkingSetSize/1024, 
							ProcessMemoryCounters.WorkingSetSize/1024 - m_markedMemSize,
							ProcessMemoryCounters.PeakWorkingSetSize/1024);
						m_markedMemSize = ProcessMemoryCounters.WorkingSetSize/1024;
					}
					::CloseHandle(hProcess);
				}
			}
			break;
		}
	}
	if (*pos == 32)//Trim space
		pos++;

	if (GetOption(LO_ShowTimeDate))
	{
		SYSTEMTIME t;
		GetLocalTime(&t);
		curLen += GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &t, _T("'['HH':'mm':'ss'] '"), &message[curLen], mLen - curLen);
		curLen--;
	}
	if (GetOption(LO_ShowTimeFromStartup))
		curLen += _sntprintf(&message[curLen], mLen - curLen, _T("%08.3fms "), (GetTickCount() - m_StartupTicks)/1000.0);
	if (GetOption(LO_ShowThreadID))
	{
		DWORD threadID = ::GetCurrentThreadId();
		if (m_MainThreadID != threadID)
			curLen += _sntprintf(&message[curLen], mLen - curLen, _T("[THR: %4X] "), threadID);
	}
	va_list list;
	va_start(list, description);
	curLen += _vsntprintf(&message[curLen], mLen - curLen, pos, list);
	va_end(list);
	//Adding CRLF at the end if needed
	if (mLen - curLen > 3)
	{
		if (message[curLen-1] == 13 || message[curLen-1] == 10)
			curLen--;
		if (message[curLen-1] == 13 || message[curLen-1] == 10)
			curLen--;
		message[curLen] = 13;
		message[curLen + 1] = 10;
		message[curLen + 2] = 0;
	}
	else
		message[mLen - 1] = 0;


	//CThreadLock lock(&m_cs, TRUE);
	if (!m_pViewer->Show(message))
	{
		delete m_pViewer;
		m_pViewer = NULL;
	}
}


AViewer* ALog::GetAutoViewer()
{
	//-----------------------------------------------
	//Check to see if ALogDebugger is already started
	//If it is is then log there anyway
	HWND hwndALogDebugger = FindWindow(NULL, _T("ALogDebugger2"));
	if (hwndALogDebugger == NULL)
	{
		//----------------------------------------------------------
		//ALogDebugger is not started. Try to start it automatically
		HKEY hKey;
		LONG ret = ::RegOpenKey(HKEY_CURRENT_USER, _T("Software\\AlogDebugger2"), &hKey);
		if (ret == ERROR_SUCCESS)
		{
			TCHAR path[MAX_PATH];
			DWORD len = MAX_PATH * sizeof(TCHAR);
			ret = ::RegQueryValueEx(hKey,
				_T("Path"),
				0,
				NULL,
				(LPBYTE) path,
				&len);
			::RegCloseKey(hKey);
			if (ret == ERROR_SUCCESS)
			{
				STARTUPINFO sI;
				memset(&sI, 0, sizeof(STARTUPINFO));
				sI.cb = sizeof(STARTUPINFO);
				sI.wShowWindow = SW_SHOWDEFAULT;
				PROCESS_INFORMATION pI;
				if (CreateProcess(path, NULL, NULL, NULL, FALSE, NULL, NULL, NULL,&sI, &pI))
				{
					Sleep(1000);
					hwndALogDebugger = FindWindow(NULL, _T("ALogDebugger2"));
				}
			}
		}
	}
	if (hwndALogDebugger)
		return new CopyDataViewer(hwndALogDebugger);


	//--------------------------------------------------------------
	//reg key is not there or generally AlogDebugget Failed to start
	//Checking if there is any Debugger active
	if (IsDebuggerPresent())
		return new DebugViewer;
	else
		return new FileViewer(_T("debug.log"));
	//--------------------------------------------------------------
	//We will start the auto window viewer
	return new AutoWindowViewer(AfxGetInstanceHandle());
}

//
//				VIEWERS
//

BOOL DebugViewer::Show(LPCTSTR description)
{
	OutputDebugString(description);
	return TRUE;
}


FileViewer::FileViewer(LPCTSTR path)
{
	m_hFile = CreateFile(path, 
		GENERIC_WRITE, 
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		NULL,
		NULL);
}
FileViewer::~FileViewer()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
}
BOOL FileViewer::Show(LPCTSTR description)
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWritten = 0;
		if (!WriteFile(m_hFile, (LPCSTR)CT2CA(description), (DWORD)_tcslen(description), &bytesWritten, NULL))
			return FALSE;//FlushFileBuffers(m_hFile);
		return bytesWritten > 0;
	}
	return FALSE;
}



//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//PrunnedFileViewer
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
#ifdef USE_PRUNNEDFILEVIEWER

PrunnedFileViewer::PrunnedFileViewer(LPCTSTR path, UINT lines):
m_hFile(NULL)
{
	//m_path = path;
	m_maxLines = lines;
	m_hFile = CreateFile(path, 
		GENERIC_WRITE, 
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		NULL,
		NULL);
}
PrunnedFileViewer::~PrunnedFileViewer()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
}
BOOL PrunnedFileViewer::Show(LPCTSTR description)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	CThreadLock lock(&m_cs, TRUE);

	DWORD dwPtr = SetFilePointer(m_hFile, 0, 0, FILE_BEGIN);
	if (dwPtr == INVALID_SET_FILE_POINTER)
	{
		DWORD dwError = GetLastError();
		if (dwError != NO_ERROR)
			return FALSE;
	}
	if (SetEndOfFile(m_hFile) == 0)
	{
		DWORD dwError = GetLastError();
		if (dwError != NO_ERROR)
			return FALSE;
	}
	while (m_debugLines.size() >= m_maxLines)
		m_debugLines.pop_front();
	WideCharToMultiByte(CP_ACP, NULL, description, -1, bf, cbfLen, 0,0);
	DWORD le = GetLastError();
	m_debugLines.push_back(bf);//Gives a ERROR_INVALID_HANDLE (6)
	if (le != GetLastError())
		SetLastError(le);
	std::list<std::string>::const_iterator it = m_debugLines.begin();
	DWORD bytesWritten = 0;
	while (it != m_debugLines.end())
	{
		DWORD bytesWritten = 0;
		const std::string& str = *it;
		if (!WriteFile(m_hFile, (LPCSTR)str.c_str(), (DWORD)str.size(), &bytesWritten, NULL))
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
			return FALSE;//FlushFileBuffers(m_hFile);
		}
		it++;
	}
	return TRUE;
}

#endif

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//MultiViewer
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
MultiViewer::MultiViewer(BOOL bAutoDelete/* = TRUE*/):
	m_bAutoDelete(bAutoDelete),
	m_viewersCount(0)
{
}
MultiViewer::~MultiViewer()
{
	if (m_bAutoDelete)
	{
		for (INT i = 0; i < m_viewersCount; i++)
			delete m_viewers[i];
	}
}

BOOL MultiViewer::AddViewer(AViewer* viewer)
{
	ASSERT(viewer != NULL);
	if (viewer != NULL && m_viewersCount < MAX_VIEWERS)
	{
		m_viewers[m_viewersCount] = viewer;
		m_viewersCount++;
		return TRUE;
	}
	return FALSE;
}

AViewer* MultiViewer::GetViewer(INT ID)
{
	ASSERT(ID < GetViewerCount());
	if (ID < GetViewerCount())
		return m_viewers[ID];
	return NULL;
}


BOOL MultiViewer::Show(LPCTSTR description)
{
	BOOL ret = FALSE;
	for (INT i = 0; i < m_viewersCount; i++)
	{
		if (m_viewers[i]->Show(description))
			ret = TRUE;//If just one succeeds then we return success
	}
	return ret;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//CopyDataViewer
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

CopyDataViewer::CopyDataViewer(HWND target)
{
	m_TargetHwnd = target;
}

BOOL CopyDataViewer::Show(LPCTSTR description)
{
	if (m_TargetHwnd)
	{
		COPYDATASTRUCT cds;
		cds.dwData = 888;
		cds.cbData = (_tcslen(description) + 1) * sizeof(TCHAR);
		cds.lpData = (LPVOID)description;
		if (::SendMessage(m_TargetHwnd, WM_COPYDATA, 0, (WPARAM)&cds))
			return TRUE;
		m_TargetHwnd = NULL;
	}
	return FALSE;
}



//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//WindowViewer
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

WindowViewer::WindowViewer(HWND target)
{
	m_TargetHwnd = target;
}

BOOL WindowViewer::Show(LPCTSTR description)
{
	if (m_TargetHwnd)
	{
		LRESULT len = ::SendMessage(m_TargetHwnd, WM_GETTEXTLENGTH, 0, 0);
		LRESULT ret = ::SendMessage(m_TargetHwnd, EM_SETSEL, (WPARAM)len, len);
		if (ret!=0)
		{
			INT l = _tcslen(description);
			::SendMessage(m_TargetHwnd, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) (LPCTSTR) description);
			return true;
		}
		else
			m_TargetHwnd = NULL; //Debugger window is closed - Don t try again
	}
	return false;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//AutoWindowViewer
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
WNDPROC gDefEditWndProc = 0;

LRESULT CALLBACK AViewerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_DESTROY: 
		{
			HWND edt = FindWindowEx(hWnd, NULL, _T("Edit"), NULL);
			if (edt != NULL)
				SetWindowLongPtr(edt, GWLP_WNDPROC, (LONG_PTR)gDefEditWndProc); 
			PostQuitMessage(0);
		}
	break;
	case AM_EXITMSG:
		DestroyWindow(hWnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND://Don t Draw the white staff
		break;
	case WM_SIZE:
		{
			HWND edt = FindWindowEx(hWnd, NULL, _T("Edit"), NULL);
			if (edt)
			{
				RECT rct;
				GetClientRect(hWnd, &rct);
				MoveWindow(edt, 0, 0, rct.right, rct.bottom, TRUE);
			}
		}
		break;
	case WM_CTLCOLOREDIT:
		::SetBkColor((HDC) wParam, (LPARAM) RGB(0,0,0));
		SetTextColor((HDC) wParam, (LPARAM) RGB(150,150,255));
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == EN_MAXTEXT)
		{
			const DWORD subtractChars = 10000;
			DWORD len = (DWORD)::SendMessage((HWND)lParam, WM_GETTEXTLENGTH, 0, 0);
			len = len - subtractChars;
			::SendMessage((HWND)lParam, EM_SETSEL, 1, subtractChars);
			::SendMessage((HWND)lParam, EM_REPLACESEL, FALSE, (LPARAM)_T(""));
			::SendMessage((HWND)lParam, EM_SETSEL, len, len);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


LRESULT CALLBACK AViewerEditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_ERASEBKGND:
		{
			RECT rct;
			GetClientRect(hWnd, &rct);
			FillRect((HDC) wParam, &rct, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
		break;
	default:
		return CallWindowProc(gDefEditWndProc, hWnd, message, wParam, lParam);
		//return DefWindowProc(hWnd, message, wParam, lParam); 

	}
	return 0;


}

AutoWindowViewer::AutoWindowViewer(HINSTANCE hInstance):
m_pWinViewer(0),
m_TrgHwnd(0),
m_hInst(0),
m_thread(0)

{
	m_hInst = hInstance;
	CWinThread* m_thread = AfxBeginThread(InitializeST, this, THREAD_PRIORITY_NORMAL, NULL/*CREATE_SUSPENDED*/);
	int i = 0;
	while (m_TrgHwnd == NULL && i++ < 10)
		Sleep(50);
	if (m_TrgHwnd)
		m_pWinViewer = new WindowViewer(m_TrgHwnd);
}
AutoWindowViewer::~AutoWindowViewer()
{
	//::SendMessage(::GetParent(GetTargetHwnd()), WM_DESTROY, NULL, NULL);
	delete m_pWinViewer;
	if (m_TrgHwnd)
	{
		HWND hWndParent = ::GetParent(m_TrgHwnd); 
		PostMessage(hWndParent, AM_EXITMSG, 0 ,0);
		Sleep(500);
	}

}

UINT AutoWindowViewer::InitializeST( LPVOID pParam )
{
	ASSERT(pParam != NULL);
	((AutoWindowViewer*) pParam)->Initialize();
	return NULL;
}


void AutoWindowViewer::Initialize()
{
	LPCTSTR const sClassName = _T("AutoWindowViewer");
	LPCTSTR const sWindowName = _T("Auto Window Viewer");
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)AViewerWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInst;
	wcex.hIcon			= LoadIcon(NULL, IDI_INFORMATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_WIN32TEST;
	wcex.lpszClassName	= sClassName;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	ATOM cls = RegisterClassEx(&wcex);
	if (cls != 0)
	{
		HWND hWnd = CreateWindow(sClassName, sWindowName, WS_OVERLAPPEDWINDOW,
			30, 30, 300, 600, NULL, NULL, m_hInst, NULL);
		if (hWnd)
		{
			::SetWindowPos(hWnd, HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREPOSITION|SWP_NOSIZE);
			HWND TargetHwnd = CreateWindowEx(0, _T("Edit"), 
				NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_WANTRETURN | ES_MULTILINE | ES_AUTOHSCROLL,
				10, 10, 100, 100, hWnd, NULL, m_hInst, NULL);
			if (TargetHwnd == NULL)
			{
				DestroyWindow(hWnd);
				return;
			}
			else
			{
				gDefEditWndProc = (WNDPROC)SetWindowLongPtr(TargetHwnd, GWLP_WNDPROC, (LONG_PTR) AViewerEditWndProc); 
				SendMessage(TargetHwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)FALSE);
				ShowWindow(hWnd, SW_SHOW);
				UpdateWindow(hWnd);
				m_TrgHwnd = TargetHwnd;
				MSG msg;
				BOOL bRet;
				while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
				{ 
					if (bRet == -1)
						return;
					else
					{
						TranslateMessage(&msg); 
						DispatchMessage(&msg); 
					}
				}
			}
		}
	}
}

BOOL AutoWindowViewer::Show(LPCTSTR description)
{
	if (m_pWinViewer!=0)
		return m_pWinViewer->Show(description);
	return FALSE;
}
